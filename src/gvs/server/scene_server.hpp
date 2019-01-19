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
#pragma once

#include "gvs/net/grpc_async_server.hpp"
#include <crossguid/guid.hpp>

#include <gvs/scene.grpc.pb.h>

#include <thread>

namespace gvs {
namespace host {

class SceneService;

class SceneServer {
public:
    explicit SceneServer(std::string server_address = "");

    grpc::Server& server();

private:
    using Service = proto::Scene::AsyncService;
    net::GrpcAsyncServer<Service> server_;

    // atomicize these
    proto::SceneItems scene_;
    proto::Messages messages_;

    gvs::net::StreamInterface<proto::Message>* message_stream_;
    gvs::net::StreamInterface<proto::SceneUpdate>* scene_stream_;

    /*
     * How items are handled based on the update request:
     *
     * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
     * | -------------- |:-----------------:| ------------------------------- | ------------------- |
     * | `gvs::send`    |      **Yes**      | **Error**                       | Creates new item    |
     * | `gvs::send`    |       *No*        | Updates item                    | **Error**           |
     * | `gvs::replace` |      **Yes**      | Replaces existing geometry*     | Creates new item    |
     * | `gvs::replace` |       *No*        | Updates item                    | **Error**           |
     * | `gvs::append`  |      **Yes**      | Appends positions to geometry** | Creates new item    |
     * | `gvs::append`  |       *No*        | Updates item                    | **Error**           |
     */

    grpc::Status safe_set_item(const proto::SceneItemInfo& info, proto::Errors* errors);
    grpc::Status replace_item(const proto::SceneItemInfo& info, proto::Errors* errors);
    grpc::Status append_to_item(const proto::SceneItemInfo& info, proto::Errors* errors);

    void add_item_and_send_update(const proto::SceneItemInfo& info, proto::Errors* errors);
    void update_item_and_send_update(const proto::SceneItemInfo& info, proto::Errors* errors);
    void remove_item_and_send_update(const proto::SceneItemInfo& info, proto::Errors* errors);
};

} // namespace host
} // namespace gvs

// //////////////////////////////////////////////////////////////////////////////////// //
// ///////////////////////////////////  TESTING  ////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////// //
#ifdef DOCTEST_LIBRARY_INCLUDED
#include <grpc++/create_channel.h>

#include <thread>

TEST_CASE("[gvs-host] test_all_ways_of_modifying_the_server") {
    std::string server_address = "0.0.0.0:50050";

    // Set up the scene server
    gvs::host::SceneServer server(server_address);

    // Set up the scene client
    gvs::net::GrpcClient<gvs::proto::Scene> client;

    auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);
    REQUIRE(client.change_server_and_wait_for_connect(server_address, deadline));

    // keeps track of scene updates received on a separate thread
    gvs::util::BlockingQueue<gvs::proto::SceneUpdate> updates;

    // Connect to the stream that delivers scene updates
    client.register_stream<gvs::proto::SceneUpdate>(
        [](std::unique_ptr<gvs::proto::Scene::Stub>& stub, grpc::ClientContext* context) {
            google::protobuf::Empty empty;
            return stub->SceneUpdates(context, empty);
        },
        [&updates](const gvs::proto::SceneUpdate& update) { updates.push_back(update); });

    /**
     * @brief Send a request, make sure it was sent successfully, return any errors.
     */
    auto send_request = [&](const gvs::proto::SceneUpdateRequest& request) {
        gvs::proto::Errors errors;

        bool successfully_sent = client.use_stub([&](auto& stub) {
            grpc::ClientContext context;
            grpc::Status status = stub->UpdateScene(&context, request, &errors);

            REQUIRE(status.ok());
        });
        REQUIRE(successfully_sent);

        return errors;
    };

    SUBCASE("no_update_set") {
        gvs::proto::SceneUpdateRequest request;
        gvs::proto::Errors errors = send_request(request);
        CHECK(errors.error_msg() == "No update set");
    }

    /*
     * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
     * | -------------- |:-----------------:| ------------------------------- | ------------------- |
     * | `gvs::send`    |      **Yes**      | **Error**                       | Creates new item    |
     */
    SUBCASE("safe_send_with_geometry") {
        // Check an item is added if it doesn't yet exist
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
            gvs::proto::Errors errors = send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was added to scene
            gvs::proto::SceneUpdate update = updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);
        }

        // Check an error is returned if the item already exists
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
            gvs::proto::Errors errors = send_request(request);

            CHECK_FALSE(errors.error_msg().empty());

            std::string error_msg_start = "Item 'safe_set_item' already exists.";
            CHECK(gvs::util::starts_with(errors.error_msg(), error_msg_start));
        }
    }

    /*
     * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
     * | -------------- |:-----------------:| ------------------------------- | ------------------- |
     * | `gvs::send`    |       *No*        | Updates item                    | **Error**           |
     */
    SUBCASE("safe_send_no_geometry") {
        // Check an error is returned if the item does not exist
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_display_info()->mutable_readable_id()->set_value("lower-case");
            gvs::proto::Errors errors = send_request(request);

            CHECK(errors.error_msg() == "Item 'safe_set_item' does not exist and no geometry was specified.");
        }

        // Add an item to the scene
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
            request.mutable_safe_set_item()->mutable_display_info()->mutable_readable_id()->set_value("lower-case");
            gvs::proto::Errors errors = send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was added to scene
            gvs::proto::SceneUpdate update = updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);
            CHECK(update.add_item().display_info().readable_id().value() == "lower-case");
        }

        // Check existing item can be updated now.
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_display_info()->mutable_readable_id()->set_value("UPPER CASE");
            gvs::proto::Errors errors = send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was updated
            gvs::proto::SceneUpdate update = updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kUpdateItem);
            CHECK(update.update_item().display_info().readable_id().value() == "UPPER CASE");
        }
    }

    /*
     * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
     * | -------------- |:-----------------:| ------------------------------- | ------------------- |
     * | `gvs::replace` |      **Yes**      | Replaces existing geometry*     | Creates new item    |
     * | `gvs::replace` |       *No*        | Updates item                    | **Error**           |
     * | `gvs::append`  |      **Yes**      | Appends positions to geometry** | Creates new item    |
     * | `gvs::append`  |       *No*        | Updates item                    | **Error**           |
     */
}

#endif
