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

// generated
#include <scene.grpc.pb.h>

// third-party
#include <crossguid/guid.hpp>
#include <grpcw/forward_declarations.hpp>

namespace gvs::server {

class SceneServer {
public:
    explicit SceneServer(const std::string& server_address = "");
    ~SceneServer();

private:
    using Service = proto::Scene::AsyncService;
    std::unique_ptr<grpcw::server::GrpcAsyncServer<Service>> server_;

    // atomicize these
    proto::SceneItems scene_;
    proto::Messages messages_;

    grpcw::server::StreamInterface<proto::Message>* message_stream_;
    grpcw::server::StreamInterface<proto::SceneUpdate>* scene_stream_;

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

} // namespace gvs::server

// //////////////////////////////////////////////////////////////////////////////////// //
// ///////////////////////////////////  TESTING  ////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////// //
#ifdef DOCTEST_LIBRARY_INCLUDED
#include "gvs/item_defaults.hpp"
#include "gvs/net/grpc_client.hpp"
#include "gvs/util/string.hpp"

#include <grpc++/create_channel.h>

#include <thread>

class SceneTestClient {

public:
    explicit SceneTestClient(const std::string& server_address) {
        auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);
        REQUIRE(grpc_client_.change_server_and_wait_for_connect(server_address, deadline));

        // Connect to the stream that delivers scene updates
        grpc_client_.register_stream<gvs::proto::SceneUpdate>(
            [](std::unique_ptr<gvs::proto::Scene::Stub>& stub, grpc::ClientContext* context) {
                google::protobuf::Empty empty;
                return stub->SceneUpdates(context, empty);
            },
            [this](const gvs::proto::SceneUpdate& update) { updates.push_back(update); });
    }

    /**
     * @brief Send a request, make sure it was sent successfully, return any errors.
     */
    gvs::proto::Errors send_request(const gvs::proto::SceneUpdateRequest& request) {
        gvs::proto::Errors errors;

        bool successfully_sent = grpc_client_.use_stub([&](auto& stub) {
            grpc::ClientContext context;
            grpc::Status status = stub->UpdateScene(&context, request, &errors);

            REQUIRE(status.ok());
        });
        REQUIRE(successfully_sent);

        return errors;
    };

    // keeps track of scene updates received on a separate thread
    gvs::util::BlockingQueue<gvs::proto::SceneUpdate> updates;

private:
    grpcw::client::GrpcClient<gvs::proto::Scene> grpc_client_;
};

TEST_CASE("[gvs-host] test_no_update_set") {
    std::string server_address = "0.0.0.0:50050";

    // Set up the scene server
    gvs::host::SceneServer server(server_address);

    // Set up the scene client
    SceneTestClient client(server_address);

    gvs::proto::SceneUpdateRequest request;
    gvs::proto::Errors errors = client.send_request(request);
    CHECK(errors.error_msg() == "No update set");
}

TEST_CASE("[gvs-host] test_safe_send") {
    std::string server_address = "0.0.0.0:50050";

    // Set up the scene server
    gvs::host::SceneServer server(server_address);

    // Set up the scene client
    SceneTestClient client(server_address);

    SUBCASE("no_update_set") {
        gvs::proto::SceneUpdateRequest request;
        gvs::proto::Errors errors = client.send_request(request);
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
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was added to scene
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);
        }

        // Check an error is returned if the item already exists
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
            gvs::proto::Errors errors = client.send_request(request);

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
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg() == "Item 'safe_set_item' does not exist and no geometry was specified.");
        }

        // Add an item to the scene
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
            request.mutable_safe_set_item()->mutable_display_info()->mutable_readable_id()->set_value("lower-case");
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was added to scene
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);
            CHECK(update.add_item().display_info().readable_id().value() == "lower-case");
        }

        // Check existing item can be updated now.
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_safe_set_item()->mutable_id()->set_value("safe_set_item");
            request.mutable_safe_set_item()->mutable_display_info()->mutable_readable_id()->set_value("UPPER CASE");
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was updated
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kUpdateItem);
            CHECK(update.update_item().display_info().readable_id().value() == "UPPER CASE");
        }
    }
}

TEST_CASE("[gvs-host] test_replace") {
    std::string server_address = "0.0.0.0:50050";

    // Set up the scene server
    gvs::host::SceneServer server(server_address);

    // Set up the scene client
    SceneTestClient client(server_address);

    /*
     * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
     * | -------------- |:-----------------:| ------------------------------- | ------------------- |
     * | `gvs::replace` |      **Yes**      | Replaces existing geometry*     | Creates new item    |
     */
    SUBCASE("replace_with_geometry") {
        // Check an item is added if it doesn't yet exist
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_replace_item()->mutable_id()->set_value("replace_item");
            gvs::proto::GeometryInfo3D* geom_info = request.mutable_replace_item()->mutable_geometry_info();
            geom_info->mutable_positions()->add_value(1.f);
            geom_info->mutable_positions()->add_value(2.f);
            geom_info->mutable_positions()->add_value(3.f);
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was added to scene
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);
            CHECK(update.add_item().geometry_info().positions().value_size() == 3);
            CHECK(update.add_item().geometry_info().positions().value(0) == 1.f);
            CHECK(update.add_item().geometry_info().positions().value(1) == 2.f);
            CHECK(update.add_item().geometry_info().positions().value(2) == 3.f);
        }

        // Check geometry is replaced if item exists
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_replace_item()->mutable_id()->set_value("replace_item");
            gvs::proto::GeometryInfo3D* geom_info = request.mutable_replace_item()->mutable_geometry_info();
            geom_info->mutable_positions()->add_value(-4.f);
            geom_info->mutable_positions()->add_value(-2.f);
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item geometry was updated
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kUpdateItem);
            CHECK(update.update_item().geometry_info().positions().value_size() == 2);
            CHECK(update.update_item().geometry_info().positions().value(0) == -4.f);
            CHECK(update.update_item().geometry_info().positions().value(1) == -2.f);
        }
    }

    /*
         * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
         * | -------------- |:-----------------:| ------------------------------- | ------------------- |
         * | `gvs::replace` |       *No*        | Updates item                    | **Error**           |
         */
    SUBCASE("safe_send_no_geometry") {
        // Check an error is returned if the item does not exist
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_replace_item()->mutable_id()->set_value("replace_item");
            request.mutable_replace_item()->mutable_display_info()->mutable_readable_id()->set_value("lower-case");
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg() == "Item 'replace_item' does not exist and no geometry was specified.");
        }

        // Add an item to the scene
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_replace_item()->mutable_id()->set_value("replace_item");
            request.mutable_replace_item()->mutable_geometry_info()->mutable_positions();
            request.mutable_replace_item()->mutable_display_info()->mutable_readable_id()->set_value("lower-case");
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was added to scene
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);
            CHECK(update.add_item().display_info().readable_id().value() == "lower-case");
        }

        // Check existing item can be updated now.
        {
            gvs::proto::SceneUpdateRequest request;
            request.mutable_replace_item()->mutable_id()->set_value("replace_item");
            request.mutable_replace_item()->mutable_display_info()->mutable_readable_id()->set_value("UPPER CASE");
            gvs::proto::Errors errors = client.send_request(request);

            CHECK(errors.error_msg().empty());

            // Item was updated
            gvs::proto::SceneUpdate update = client.updates.pop_front();
            CHECK(update.update_case() == gvs::proto::SceneUpdate::kUpdateItem);
            CHECK(update.update_item().display_info().readable_id().value() == "UPPER CASE");
        }
    }
}

TEST_CASE("[gvs-host] check_defaults_are_set_properly") {
    std::string server_address = "0.0.0.0:50050";

    // Set up the scene server
    gvs::host::SceneServer server(server_address);

    // Set up the scene client
    SceneTestClient client(server_address);

    // Add an item with no default values
    gvs::proto::SceneUpdateRequest request;
    request.mutable_safe_set_item()->mutable_id()->set_value("unique_item_id");
    request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
    gvs::proto::Errors errors = client.send_request(request);

    CHECK(errors.error_msg().empty());

    // Item was added to scene
    gvs::proto::SceneUpdate update = client.updates.pop_front();
    CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);

    const gvs::proto::DisplayInfo& display_info = update.add_item().display_info();

    CHECK(display_info.readable_id().value() == "unique_item_id");
    CHECK(display_info.geometry_format().value() == gvs::default_geom_format);
    CHECK(display_info.transformation().data_size() == 16);
    CHECK(std::equal(display_info.transformation().data().data(),
                     display_info.transformation().data().data() + 16,
                     gvs::default_transformation.data()));
    CHECK(display_info.uniform_color().x() == gvs::default_color[0]);
    CHECK(display_info.uniform_color().y() == gvs::default_color[1]);
    CHECK(display_info.uniform_color().z() == gvs::default_color[2]);
    CHECK(display_info.coloring().value() == gvs::default_coloring);
    CHECK(display_info.shading().value_case() == gvs::proto::Shading::kUniformColor);
}

TEST_CASE("[gvs-host] check_lambertian_defaults_are_set_properly") {
    std::string server_address = "0.0.0.0:50050";

    // Set up the scene server
    gvs::host::SceneServer server(server_address);

    // Set up the scene client
    SceneTestClient client(server_address);

    // Add an item with empty lambertian shading
    gvs::proto::SceneUpdateRequest request;
    request.mutable_safe_set_item()->mutable_id()->set_value("unique_item_id");
    request.mutable_safe_set_item()->mutable_geometry_info()->mutable_positions();
    request.mutable_safe_set_item()->mutable_display_info()->mutable_shading()->mutable_lambertian();
    gvs::proto::Errors errors = client.send_request(request);

    CHECK(errors.error_msg().empty());

    // Item was added to scene
    gvs::proto::SceneUpdate update = client.updates.pop_front();
    CHECK(update.update_case() == gvs::proto::SceneUpdate::kAddItem);

    const gvs::proto::DisplayInfo& display_info = update.add_item().display_info();
    CHECK(display_info.shading().value_case() == gvs::proto::Shading::kLambertian);

    const gvs::proto::LambertianShading& shading = display_info.shading().lambertian();
    CHECK(shading.light_direction().x() == gvs::default_light_direction[0]);
    CHECK(shading.light_direction().y() == gvs::default_light_direction[1]);
    CHECK(shading.light_direction().z() == gvs::default_light_direction[2]);

    CHECK(shading.light_color().x() == gvs::default_light_color[0]);
    CHECK(shading.light_color().y() == gvs::default_light_color[1]);
    CHECK(shading.light_color().z() == gvs::default_light_color[2]);

    CHECK(shading.ambient_color().x() == gvs::default_ambient_color[0]);
    CHECK(shading.ambient_color().y() == gvs::default_ambient_color[1]);
    CHECK(shading.ambient_color().z() == gvs::default_ambient_color[2]);
}

#endif
