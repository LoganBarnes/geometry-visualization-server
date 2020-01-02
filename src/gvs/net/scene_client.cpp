// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2019 Logan Barnes - All Rights Reserved
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
#include "scene_client.hpp"
#include "type_conversions.hpp"

namespace gvs {
namespace net {

SceneClient::~SceneClient() = default;

auto SceneClient::set_seed(std::random_device::result_type /*seed*/) -> SceneClient& {
    return *this;
}

auto SceneClient::clear() -> void {
    grpc::ClientContext     context;
    google::protobuf::Empty empty;

    auto status = stub_->Clear(&context, empty, &empty);
    if (!status.ok()) {
        throw std::runtime_error(status.error_message());
    }
}

auto SceneClient::actually_add_item(SceneItemInfoSetter&& info) -> util11::Result<gvs::SceneId> {
    grpc::ClientContext context;
    net::SceneItemInfo  request  = to_proto(info);
    net::SceneIdResult  response = {};

    auto status = stub_->AddItem(&context, request, &response);
    if (!status.ok()) {
        return util11::Error{status.error_message()};
    }

    if (response.has_errors()) {
        return util11::Error{response.errors().error_msg()};
    }

    return from_proto(response.value());
}

auto SceneClient::actually_update_item(gvs::SceneId const& item_id, SceneItemInfoSetter&& info) -> util11::Error {
    grpc::ClientContext context;

    net::SceneItemInfoWithId request = {};
    *request.mutable_id()            = to_proto(item_id);
    *request.mutable_info()          = to_proto(info);

    net::Result response = {};

    auto status = stub_->UpdateItem(&context, request, &response);
    if (!status.ok()) {
        return util11::Error{status.error_message()};
    }

    if (response.has_errors()) {
        return util11::Error{response.errors().error_msg()};
    }

    return util11::success();
}

auto SceneClient::actually_append_to_item(gvs::SceneId const& item_id, SceneItemInfoSetter&& info) -> util11::Error {
    grpc::ClientContext context;

    net::SceneItemInfoWithId request = {};
    *request.mutable_id()            = to_proto(item_id);
    *request.mutable_info()          = to_proto(info);

    net::Result response = {};

    auto status = stub_->AppendToItem(&context, request, &response);
    if (!status.ok()) {
        return util11::Error{status.error_message()};
    }

    if (response.has_errors()) {
        return util11::Error{response.errors().error_msg()};
    }

    return util11::success();
}

auto SceneClient::items() const -> gvs::SceneItems const& {
    grpc::ClientContext     context;
    google::protobuf::Empty empty;

    auto item_reader = stub_->GetItems(&context, empty);

    most_recent_item_list_.clear();

    net::SceneItemInfoWithId info_with_id = {};
    while (item_reader->Read(&info_with_id)) {
        //        auto&& id   = from_proto(info_with_id.id());
        //        auto&& info = from_proto(info_with_id.info());
        //        most_recent_item_list_.emplace(id, info);
    }

    auto status = item_reader->Finish();
    if (!status.ok()) {
        throw std::runtime_error(status.error_message());
    }

    return most_recent_item_list_;
}

} // namespace net
} // namespace gvs
