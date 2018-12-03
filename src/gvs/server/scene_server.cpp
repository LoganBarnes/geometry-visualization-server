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
#include "gvs/server/scene_server.hpp"
#include "scene_server.hpp"

#include <grpcpp/server_builder.h>
#include <grpcpp/security/server_credentials.h>

namespace gvs {
namespace host {

SceneServer::SceneServer(std::string server_address)
    : server_(std::make_shared<gvs::proto::Scene::AsyncService>(), server_address) {

    gvs::net::StreamInterface<gvs::proto::Message>* message_stream
        = server_.register_async_stream(&Service::RequestMessageUpdates,
                                        [](const google::protobuf::Empty& /*ignored*/) {});

    gvs::net::StreamInterface<gvs::proto::SceneUpdate>* scene_stream
        = server_.register_async_stream(&Service::RequestSceneUpdates,
                                        [](const google::protobuf::Empty& /*ignored*/) {});

    server_.register_async(&Service::RequestSendMessage,
                           [this, message_stream](const gvs::proto::Message& message, gvs::proto::SceneResponse*) {
                               messages_.add_messages()->CopyFrom(message);
                               message_stream->write(message);
                               return grpc::Status::OK;
                           });

    server_.register_async(&Service::RequestGetAllMessages,
                           [this](const google::protobuf::Empty& /*empty*/, gvs::proto::Messages* messages) {
                               messages->CopyFrom(messages_);
                               return grpc::Status::OK;
                           });

    server_.register_async(&Service::RequestUpdateScene,
                           [scene_stream](const gvs::proto::SceneUpdate& update, gvs::proto::SceneResponse* response) {
                               switch (update.update_type_case()) {

                               case proto::SceneUpdate::kAddItem:
                                   scene_stream->write(update);
                                   break;

                               case proto::SceneUpdate::UPDATE_TYPE_NOT_SET:
                                   response->set_error_msg("No update set");
                                   break;
                               }
                               return grpc::Status::OK;
                           });
}

grpc::Server& SceneServer::server() {
    return server_.server();
}

} // namespace host
} // namespace gvs
