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
//#include <gvs/log/geometry_stream.hpp>

// project
#include "gvs/log/log_params.hpp"
#include "gvs/log/send.hpp"

// generated
#include <gvs/scene.grpc.pb.h>

// third party
#include <crossguid/guid.hpp>
#include <grpcpp/create_channel.h>

// standard
#include <vector>

namespace gvs {
namespace log {

class SceneItem {
public:
    SceneItem() { info_.mutable_id()->set_value(xg::newGuid().str()); }

    SceneItem& operator<<(const std::vector<float>& positions) {
        *(info_.mutable_geometry_info()->mutable_positions()->mutable_value()) = {positions.begin(), positions.end()};
        return *this;
    }

private:
    gvs::proto::SceneItemInfo info_;
};

//template <ParamType PT, typename Func>
//struct Param {
//    Func move_func;
//};

enum class SendType {
    safe,
    replace,
    append,
};

struct ItemStream {

    explicit ItemStream(std::string id, gvs::proto::Scene::Stub* stub) : id_(std::move(id)), stub_(stub) {
        info_.mutable_id()->set_value(id_);
    }

    ~ItemStream() = default;

    void send(SendType type) {
        if (stub_) {
            gvs::proto::SceneUpdate update;

            switch (type) {
            case SendType::safe:
                update.mutable_safe_set_item()->CopyFrom(info_);
                break;

            case SendType::replace:
                update.mutable_replace_item()->CopyFrom(info_);
                break;

            case SendType::append:
                update.mutable_append_to_item()->CopyFrom(info_);
                break;
            }

            grpc::ClientContext context;
            gvs::proto::Errors errors;

            grpc::Status status = stub_->UpdateScene(&context, update, &errors);

            if (not status.ok()) {
                error_message_ = status.error_message();

            } else if (not errors.error_msg().empty()) {
                error_message_ = errors.error_msg();

            } else {
                error_message_ = "";
            }
        }

        info_.Clear();
    }

    bool success() const { return error_message_.empty(); }

    const std::string& error_message() const { return error_message_; }

    template <typename Functor>
    ItemStream& operator<<(Functor&& functor) {
        std::string error_name = functor(&info_);
        if (not error_name.empty()) {
            throw std::invalid_argument(error_name + " is already set");
        }
        return *this;
    }

    ItemStream& operator<<(ItemStream& (*send_func)(ItemStream&)) { return send_func(*this); }

    const std::string id_;
    std::string error_message_ = "";

    gvs::proto::SceneItemInfo info_;
    gvs::proto::Scene::Stub* stub_;
};

ItemStream& send(ItemStream& sender);
ItemStream& replace(ItemStream& sender);
ItemStream& append(ItemStream& sender);

ItemStream& send(ItemStream& sender) {
    sender.send(SendType::safe);
    return sender;
}

ItemStream& replace(ItemStream& sender) {
    sender.send(SendType::replace);
    return sender;
}

ItemStream& append(ItemStream& sender) {
    sender.send(SendType::append);
    return sender;
}

class Scene {
public:
    explicit Scene(const std::string& server_address) : Scene(server_address, std::chrono::seconds(3)) {}

    template <typename Rep, typename Period>
    Scene(const std::string& server_address, const std::chrono::duration<Rep, Period>& max_connection_wait_duration) {

        if (server_address.empty()) {
            std::cout << "No server address provided. Ignoring stream requests." << std::endl;

        } else {
            channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());

            if (channel_->WaitForConnected(std::chrono::system_clock::now() + max_connection_wait_duration)) {
                stub_ = gvs::proto::Scene::NewStub(channel_);

            } else {
                std::cerr << "Failed to connect to " << server_address << ". No messages will be sent." << std::endl;
            }
        }
    }

    ItemStream stream(const std::string& id = "") {
        if (id.empty()) {
            return ItemStream(generate_uuid(), stub_.get());
        }
        return ItemStream(id, stub_.get());
    }

    std::string generate_uuid() {
        xg::Guid uuid;
        return uuid.str();
    }

private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<gvs::proto::Scene::Stub> stub_;
    gvs::proto::SceneItemInfo info_;
};

} // namespace log
} // namespace gvs

int main(int argc, char* argv[]) {
    std::string server_address = "0.0.0.0:50055";

    if (argc > 1) {
        server_address = argv[1];
    }

    using namespace std::chrono_literals;

    //    gvs::log::Scene scene(server_address, 3s);
    gvs::log::Scene scene("");

    gvs::log::ItemStream stream = scene.stream() << gvs::positions_3d({}) << gvs::log::send;
    //    gvs::log::ItemStream stream = scene.stream() << gvs::positions_3d({});

    stream << gvs::positions_3d({}) << gvs::normals_3d({}) << gvs::tex_coords_3d({}) << gvs::vertex_colors_3d({});
    //    stream << gvs::points({}) << gvs::line_strip({});
    stream << gvs::indices<gvs::proto::GeometryFormat::TRIANGLE_FAN>({});

    std::cout << stream.id_ << std::endl;
}
