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
#include "gvs/item_defaults.hpp"
#include "scene_server.hpp"

#include <grpc++/security/server_credentials.h>
#include <grpc++/server_builder.h>

namespace gvs {
namespace host {

namespace {

void set_display_defaults(proto::SceneItemInfo* info) {
    proto::DisplayInfo* display_info = info->mutable_display_info();

    if (not display_info->has_readable_id()) {
        display_info->mutable_readable_id()->set_value(info->id().value());
    }

    if (not display_info->has_geometry_format()) {
        display_info->mutable_geometry_format()->set_value(default_geom_format);
    }

    if (not display_info->has_transformation()) {
        (*display_info->mutable_transformation()->mutable_data())
            = {std::begin(default_transformation), std::end(default_transformation)};
    }

    if (not display_info->has_uniform_color()) {
        display_info->mutable_uniform_color()->set_x(default_color[0]);
        display_info->mutable_uniform_color()->set_y(default_color[1]);
        display_info->mutable_uniform_color()->set_z(default_color[2]);
    }

    if (not display_info->has_coloring()) {
        display_info->mutable_coloring()->set_value(default_coloring);
    }

    if (not display_info->has_shading()) {
        display_info->mutable_shading()->mutable_uniform_color();

    } else if (display_info->shading().has_lambertian()) {
        proto::LambertianShading* lambertian = display_info->mutable_shading()->mutable_lambertian();

        if (not lambertian->has_light_direction()) {
            lambertian->mutable_light_direction()->set_x(default_light_direction[0]);
            lambertian->mutable_light_direction()->set_y(default_light_direction[1]);
            lambertian->mutable_light_direction()->set_z(default_light_direction[2]);
        }

        if (not lambertian->has_light_color()) {
            lambertian->mutable_light_color()->set_x(default_light_color[0]);
            lambertian->mutable_light_color()->set_y(default_light_color[1]);
            lambertian->mutable_light_color()->set_z(default_light_color[2]);
        }

        if (not lambertian->has_ambient_color()) {
            lambertian->mutable_ambient_color()->set_x(default_ambient_color[0]);
            lambertian->mutable_ambient_color()->set_y(default_ambient_color[1]);
            lambertian->mutable_ambient_color()->set_z(default_ambient_color[2]);
        }
    }
}

void update_display_defaults(proto::SceneItemInfo* old_info, const proto::SceneItemInfo& new_info) {
    if (new_info.has_display_info()) {
        proto::DisplayInfo* old_display_info = old_info->mutable_display_info();
        const proto::DisplayInfo& new_display_info = new_info.display_info();

        if (new_display_info.has_readable_id()) {
            old_display_info->mutable_readable_id()->CopyFrom(new_display_info.readable_id());
        }

        if (new_display_info.has_geometry_format()) {
            old_display_info->mutable_geometry_format()->CopyFrom(new_display_info.geometry_format());
        }

        if (new_display_info.has_transformation()) {
            old_display_info->mutable_transformation()->CopyFrom(new_display_info.transformation());
        }

        if (new_display_info.has_uniform_color()) {
            old_display_info->mutable_uniform_color()->CopyFrom(new_display_info.uniform_color());
        }

        if (new_display_info.has_coloring()) {
            old_display_info->mutable_coloring()->CopyFrom(new_display_info.coloring());
        }

        if (new_display_info.has_shading()) {
            old_display_info->mutable_shading()->CopyFrom(new_display_info.shading());
        }
    }
}

} // namespace

SceneServer::SceneServer(std::string server_address)
    : server_(std::make_shared<proto::Scene::AsyncService>(), server_address) {

    /*
     * Streaming calls
     */
    message_stream_ = server_.register_async_stream(&Service::RequestMessageUpdates,
                                                    [](const google::protobuf::Empty& /*ignored*/) {});

    scene_stream_ = server_.register_async_stream(&Service::RequestSceneUpdates,
                                                  [](const google::protobuf::Empty& /*ignored*/) {});

    /*
     * Getters for current state
     */
    server_.register_async(&Service::RequestGetAllMessages,
                           [this](const google::protobuf::Empty& /*empty*/, proto::Messages* messages) {
                               messages->CopyFrom(messages_);
                               return grpc::Status::OK;
                           });

    server_.register_async(&Service::RequestGetAllItems,
                           [this](const google::protobuf::Empty& /*empty*/, proto::SceneItems* scene) {
                               scene->CopyFrom(scene_);
                               return grpc::Status::OK;
                           });

    /*
     * Setters for current state
     */
    server_.register_async(&Service::RequestSetAllItems,
                           [this](const proto::SceneItems& scene, proto::Errors* /*errors*/) {
                               // TODO: Error check and set errors if necessary
                               scene_.CopyFrom(scene);
                               return grpc::Status::OK;
                           });

    /*
     * Update requests
     */
    server_.register_async(&Service::RequestSendMessage,
                           [this](const proto::Message& message, proto::Errors* /*errors*/) {
                               messages_.add_messages()->CopyFrom(message);
                               message_stream_->write(message);
                               return grpc::Status::OK;
                           });

    server_.register_async(&Service::RequestUpdateScene,
                           [this](const proto::SceneUpdateRequest& update_request, proto::Errors* errors) {
                               switch (update_request.update_case()) {

                               case proto::SceneUpdateRequest::kSafeSetItem:
                                   return safe_set_item(update_request.safe_set_item(), errors);

                               case proto::SceneUpdateRequest::kReplaceItem:
                                   return replace_item(update_request.replace_item(), errors);

                               case proto::SceneUpdateRequest::kAppendToItem:
                                   return append_to_item(update_request.append_to_item(), errors);

                               case proto::SceneUpdateRequest::kClearAll: {
                                   scene_.clear_items();
                                   proto::SceneUpdate update;
                                   update.mutable_reset_all_items()->CopyFrom(scene_);
                                   scene_stream_->write(update);
                               } break;

                               case proto::SceneUpdateRequest::UPDATE_NOT_SET:
                                   errors->set_error_msg("No update set");
                                   break;
                               }

                               return grpc::Status::OK;
                           });
}

grpc::Server& SceneServer::server() {
    return server_.server();
}

/*
 * See the full table in "scene_server.hpp"
 *
 * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
 * | -------------- |:-----------------:| ------------------------------- | ------------------- |
 * | `gvs::send`    |      **Yes**      | **Error**                       | Creates new item    |
 * | `gvs::send`    |       *No*        | Updates item                    | **Error**           |
 */
grpc::Status SceneServer::safe_set_item(const proto::SceneItemInfo& info, proto::Errors* errors) {
    std::string id = info.id().value();

    if (info.has_geometry_info()) {

        if (util::has_key(scene_.items(), id)) {
            errors->set_error_msg(
                "Item '" + id
                + "' already exists. If this is expected, try using gvs::replace or gvs::append to modify geometry");
            return grpc::Status::OK;
        }

        add_item_and_send_update(info, errors);

    } else {

        if (not util::has_key(scene_.items(), id)) {
            errors->set_error_msg("Item '" + id + "' does not exist and no geometry was specified.");
            return grpc::Status::OK;
        }

        update_item_and_send_update(info, errors);
    }

    return grpc::Status::OK;
}

/*
 * See the full table in "scene_server.hpp"
 *
 * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
 * | -------------- |:-----------------:| ------------------------------- | ------------------- |
 * | `gvs::replace` |      **Yes**      | Replaces existing geometry      | Creates new item    |
 * | `gvs::replace` |       *No*        | Updates item                    | **Error**           |
 */
grpc::Status SceneServer::replace_item(const proto::SceneItemInfo& info, proto::Errors* errors) {
    std::string id = info.id().value();

    if (info.has_geometry_info()) {

        if (util::has_key(scene_.items(), id)) {
            scene_.mutable_items()->at(id).mutable_geometry_info()->CopyFrom(info.geometry_info());
            update_item_and_send_update(info, errors);
            return grpc::Status::OK;
        }

        add_item_and_send_update(info, errors);

    } else {

        if (not util::has_key(scene_.items(), id)) {
            errors->set_error_msg("Item '" + id + "' does not exist and no geometry was specified.");
            return grpc::Status::OK;
        }

        update_item_and_send_update(info, errors);
    }

    return grpc::Status::OK;
}

/*
 * See the full table in "scene_server.hpp"
 *
 * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
 * | -------------- |:-----------------:| ------------------------------- | ------------------- |
 * | `gvs::append`  |      **Yes**      | Appends positions to geometry** | Creates new item    |
 * | `gvs::append`  |       *No*        | Updates item                    | **Error**           |
 */
grpc::Status host::SceneServer::append_to_item(const proto::SceneItemInfo& info, proto::Errors* errors) {
    std::string id = info.id().value();

    if (util::has_key(scene_.items(), id)) {
        if (info.has_geometry_info()) {
            errors->set_error_msg("Appending geometry is not supported yet");
            return grpc::Status::OK;
        }

        proto::SceneUpdate update;
        update.mutable_update_item()->CopyFrom(info);

        scene_stream_->write(update);

    } else {
        // Item doesn't yet exist. Add it.
        // TODO: Error check? (has_geometry, etc.)
        scene_.mutable_items()->insert({id, info});
        // TODO: Handle parent and children updates

        set_display_defaults(&scene_.mutable_items()->at(id));

        proto::SceneUpdate update;
        update.mutable_add_item()->CopyFrom(scene_.mutable_items()->at(id));

        scene_stream_->write(update);
    }

    return grpc::Status::OK;
}

void host::SceneServer::add_item_and_send_update(const proto::SceneItemInfo& info, proto::Errors* /*errors*/) {
    const std::string& id = info.id().value();

    // TODO: Error check (has correct geometry, etc.)
    scene_.mutable_items()->insert({id, info});
    // TODO: Handle parent and children updates

    set_display_defaults(&scene_.mutable_items()->at(id));

    proto::SceneUpdate update;
    update.mutable_add_item()->CopyFrom(scene_.mutable_items()->at(id));
    scene_stream_->write(update);
}

void host::SceneServer::update_item_and_send_update(const proto::SceneItemInfo& info, proto::Errors* /*errors*/) {
    const std::string& id = info.id().value();

    update_display_defaults(&scene_.mutable_items()->at(id), info);

    // TODO: Handle parent and children updates

    proto::SceneUpdate update;
    update.mutable_update_item()->CopyFrom(info);

    scene_stream_->write(update);
}

void host::SceneServer::remove_item_and_send_update(const proto::SceneItemInfo& /*info*/, proto::Errors* /*errors*/) {
    // TODO
}

} // namespace host
} // namespace gvs
