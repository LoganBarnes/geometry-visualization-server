// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2018 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/net/grpc_async_server.hpp"
#include "test-util/test_proto_util.hpp"

#include <testing/testing.grpc.pb.h>

#include <gmock/gmock.h>
#include <grpcpp/create_channel.h>

#include <experimental/optional>
#include <thread>

template class gvs::net::GrpcAsyncServer<gvs::test::proto::Test::AsyncService>;

namespace {

class EchoService : public gvs::test::proto::Test::Service {
public:
    EchoService() = default;

    grpc::Status echo(grpc::ServerContext* /*context*/,
                      const gvs::test::proto::TestMessage* request,
                      gvs::test::proto::TestMessage* response) override {
        response->CopyFrom(*request);
        return grpc::Status::OK;
    }
};

class EchoClient {
public:
    explicit EchoClient(const std::string& address)
        : channel_(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()))
        , stub_(gvs::test::proto::Test::NewStub(channel_)) {}

    const std::unique_ptr<gvs::test::proto::Test::Stub>& stub() const { return stub_; }

private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<gvs::test::proto::Test::Stub> stub_;
};

test(MultiClientGrpcServerTests, run_inprocess_echo_rpc_call) {
    std::string address = "0.0.0.0:50051";

    // Set up the server
    using Service = gvs::test::proto::Test::AsyncService;
    gvs::net::GrpcAsyncServer<Service> server(std::make_shared<Service>(), address);

    server.register_async(&Service::Requestecho,
                          [&](const gvs::test::proto::TestMessage& request, gvs::test::proto::TestMessage* response) {
                              response->CopyFrom(request); // Respond with the same exact message that was received
                              return grpc::Status::OK;
                          });

    // Set up the client using the in-process channel
    auto stub = gvs::test::proto::Test::NewStub(server.server().InProcessChannel(grpc::ChannelArguments{}));

    // Send a message and check for an identical response
    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    ASSERT_TRUE(status.ok());
    ASSERT_EQ(response.msg(), test_msg);
}

test(MultiClientGrpcServerTests, streaming_client_cancelled) {
    std::string address = "0.0.0.0:50051";

    using Service = gvs::test::proto::Test::AsyncService;

    gvs::net::GrpcAsyncServer<Service> server(std::make_shared<Service>(), address);
    std::thread::id thread_id1, thread_id2;

    server.register_async(&Service::Requestecho,
                          [&](const gvs::test::proto::TestMessage& request, gvs::test::proto::TestMessage* response) {
                              thread_id1 = std::this_thread::get_id();
                              response->CopyFrom(request);
                              return grpc::Status::OK;
                          });

    gvs::util::BlockingQueue<gvs::test::proto::TestMessage> input;

    gvs::net::StreamInterface<gvs::test::proto::TestMessage>* update_stream
        = server.register_async_stream(&Service::Requestendless_echo_stream,
                                       [&](const gvs::test::proto::TestMessage& request) {
                                           thread_id2 = std::this_thread::get_id();
                                           input.push_back(request);
                                       });

    // All callbacks should be on the same thread.
    ASSERT_EQ(thread_id1, thread_id2);

    std::atomic_bool keep_going(true);

    std::thread update_thread([&] {
        gvs::test::proto::TestMessage request = input.pop_front();

        // Using a for loop to prevent an infinite while loop in the test
        for (int i = 0; i < 10000 and keep_going.load(); ++i) {
            update_stream->write(request); // send request as response (echo)
        }
    });

    auto client = std::make_unique<EchoClient>(address);
    auto client2 = std::make_unique<EchoClient>(address);

    // Unary RPC call
    {
        grpc::ClientContext context;
        gvs::test::proto::TestMessage request;
        request.set_msg("asdfqwerzxcvbnmghtyuioplkj");
        gvs::test::proto::TestMessage response;
        client->stub()->echo(&context, request, &response);
        ASSERT_EQ(request, response);
    }

    // Streaming RPC call
    {
        grpc::ClientContext context;
        gvs::test::proto::TestMessage request;
        request.set_msg("asdfqwerzxcvbnmghtyuioplkj");
        std::unique_ptr<grpc::ClientReader<gvs::test::proto::TestMessage>> responder
            = client2->stub()->endless_echo_stream(&context, request);

        gvs::test::proto::TestMessage response;
        for (int i = 0; responder->Read(&response); ++i) {
            ASSERT_EQ(request, response);

            if (i >= 50) {
                context.TryCancel();
            }
        }

        grpc::Status status = responder->Finish();
        ASSERT_FALSE(status.ok());
        ASSERT_EQ(status.error_message(), "Cancelled");
    }

    // Make sure the update loop can continue and exit
    input.push_back({});
    keep_going.store(false);

    update_thread.join();
}

test(MultiClientGrpcServerTests, mutiple_threads_writing) {
    std::string address = "0.0.0.0:50051";

    using Service = gvs::test::proto::Test::AsyncService;
    gvs::net::GrpcAsyncServer<Service> server(std::make_shared<Service>(), address);

    gvs::net::StreamInterface<gvs::test::proto::TestMessage>* update_stream
        = server.register_async_stream(&Service::Requestendless_echo_stream,
                                       [&](const gvs::test::proto::TestMessage&) {});

    gvs::test::proto::TestMessage request;
    request.set_msg("aewva2    n938febq1ifjnf9 we  p4qoq;3qfw");

    std::atomic_bool keep_writing(true);

    auto write_loop = [&] {
        // Using a for loop to prevent an infinite while loop in the test
        for (int i = 0; i < 10000 and keep_writing.load(); ++i) {
            update_stream->write(request); // send request as response (echo)
        }
    };

    std::thread update_thread1(write_loop);
    std::thread update_thread2(write_loop);
    std::thread update_thread3(write_loop);
    std::thread update_thread4(write_loop);

    auto client = std::make_unique<EchoClient>(address);

    grpc::ClientContext context;
    std::unique_ptr<grpc::ClientReader<gvs::test::proto::TestMessage>> responder
        = client->stub()->endless_echo_stream(&context, request);

    gvs::test::proto::TestMessage response;
    for (int i = 0; responder->Read(&response); ++i) {
        ASSERT_EQ(request, response);

        if (i >= 50) {
            context.TryCancel();
        }
    }

    grpc::Status status = responder->Finish();
    ASSERT_FALSE(status.ok());
    ASSERT_EQ(status.error_message(), "Cancelled");

    keep_writing.store(false);
    update_thread1.join();
    update_thread2.join();
    update_thread3.join();
    update_thread4.join();
}

} // namespace
