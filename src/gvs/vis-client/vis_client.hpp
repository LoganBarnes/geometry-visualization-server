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

#include "gvs/vis-client/imgui_magnum_application.hpp"
#include "gvs/net/grpc_client_state.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/blocking_queue.hpp"
#include "gvs/forward_declarations.hpp"

#include <gvs/scene.grpc.pb.h>

#include <grpcpp/channel.h>
#include <grpcpp/completion_queue.h>

#include <thread>

namespace gvs {
namespace vis {

class VisClient : public ImGuiMagnumApplication {
public:
    explicit VisClient(const std::string& initial_host_address, const Arguments& arguments);
    ~VisClient() override;

private:
    void update() override;
    void render() const override;
    void configure_gui() override;

    void process_message_update(const proto::Message& message);

    std::string gl_version_str_;
    std::string gl_renderer_str_;

    std::string server_address_input_ = "address:port";
    std::string error_message_;

    std::unique_ptr<net::GrpcClient<gvs::proto::Scene>> grpc_client_;
    bool wrap_text_ = false;
    util::AtomicData<gvs::proto::Messages> messages_;

    std::string message_id_input_;
    std::string message_content_input_;
};

} // namespace vis
} // namespace gvs
