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

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>

namespace gvs {
namespace host {

SceneServer::SceneServer(std::string server_address)
    : server_(std::make_shared<gvs::proto::Scene::AsyncService>(), server_address) {

    /*
     * Streaming calls
     */
    gvs::net::StreamInterface<gvs::proto::Message>* message_stream
        = server_.register_async_stream(&Service::RequestMessageUpdates,
                                        [](const google::protobuf::Empty& /*ignored*/) {});

    gvs::net::StreamInterface<gvs::proto::SceneUpdate>* scene_stream
        = server_.register_async_stream(&Service::RequestSceneUpdates,
                                        [](const google::protobuf::Empty& /*ignored*/) {});

    /*
     * Getters for current state
     */
    server_.register_async(&Service::RequestGetAllMessages,
                           [this](const google::protobuf::Empty& /*empty*/, gvs::proto::Messages* messages) {
                               messages->CopyFrom(messages_);
                               return grpc::Status::OK;
                           });

    server_.register_async(&Service::RequestGetAllItems,
                           [this](const google::protobuf::Empty& /*empty*/, gvs::proto::SceneItems* items) {
                               items->CopyFrom(items_);
                               return grpc::Status::OK;
                           });

    /*
     * Setters for current state
     */
    server_.register_async(&Service::RequestSetAllItems,
                           [this](const gvs::proto::SceneItems& items, gvs::proto::Errors* /*errors*/) {
                               // TODO: Error check and set errors if necessary
                               items_.CopyFrom(items);
                               return grpc::Status::OK;
                           });

    /*
     * Update requests
     */
    server_.register_async(&Service::RequestSendMessage,
                           [this, message_stream](const gvs::proto::Message& message, gvs::proto::Errors* /*errors*/) {
                               messages_.add_messages()->CopyFrom(message);
                               message_stream->write(message);
                               return grpc::Status::OK;
                           });

    server_.register_async(&Service::RequestUpdateScene,
                           [scene_stream, this](const gvs::proto::SceneUpdate& update, gvs::proto::Errors* errors) {
                               switch (update.update_case()) {

                               case proto::SceneUpdate::kSafeSetItem: {
                                   std::string id = update.safe_set_item().id().value();

                                   if (util::has_key(item_ids_, id)) {
                                       errors->set_error_msg("Item with ID already exists");
                                       return grpc::Status::OK;
                                   }

                                   item_ids_.emplace(id, items_.items_size());
                                   items_.add_items()->CopyFrom(update.safe_set_item());
                                   // TODO: Handle parent and children updates

                                   scene_stream->write(update);
                               } break;

                               case proto::SceneUpdate::kReplaceItem:
                                   break;
                               case proto::SceneUpdate::kAppendToItem:
                                   break;

                               case proto::SceneUpdate::UPDATE_NOT_SET:
                                   errors->set_error_msg("No update set");
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
