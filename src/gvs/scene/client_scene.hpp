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
#pragma once

// project
#include "scene.hpp"

// standard
#include <chrono>

namespace gvs {
namespace scene {

class ClientScene : public Scene {
public:
    /// \brief Handles the server connection for geometry streams
    ///
    ///        If 'server_address' is an empty string, no connection attempt will be made.
    ///        Defaults to a maximum 5 second wait time when attempting to connect.
    ///
    /// \param server_address - the logger will attempt to connect to this address
    explicit ClientScene(const std::string& server_address);

    template <typename Rep, typename Period>
    explicit ClientScene(const std::string&                        server_address,
                         const std::chrono::duration<Rep, Period>& max_connection_wait_duration);

    ~ClientScene() override;

    /*
     * Start `Scene` functions
     */
    auto set_seed(std::random_device::result_type seed) -> ClientScene& override;
    auto clear() -> void override;

private:
    auto actually_add_item(SceneItemInfo&& info) -> SceneID override;
    auto actually_update_item(SceneID const& item_id, SceneItemInfo&& info) -> void override;
    auto actually_append_to_item(SceneID const& item_id, SceneItemInfo&& info) -> void override;

    auto items() const -> SceneItems const& override;
    /*
     * End `Scene` functions
     */

    std::mt19937 generator_; ///< Used to generate SceneIDs
    SceneItems   items_; ///< The map of all the items in the scene
    // Client client_; ///< The client used to send and receive scene updates
};

template <typename Rep, typename Period>
ClientScene::ClientScene(const std::string& server_address,
                         const std::chrono::duration<Rep, Period>& /*max_connection_wait_duration*/)
    : generator_(std::random_device{}()) {

    if (server_address.empty()) {
        std::cout << "No server address provided. Ignoring scene updates." << std::endl;

    } else {
        //        grpc::ChannelArguments channel_args;
        //        channel_args.SetMaxReceiveMessageSize(std::numeric_limits<int>::max());
        //
        //        channel_ = grpc::CreateCustomChannel(server_address, grpc::InsecureChannelCredentials(), channel_args);
        //
        //        if (channel_->WaitForConnected(std::chrono::system_clock::now() + max_connection_wait_duration)) {
        //            stub_ = proto::Scene::NewStub(channel_);
        //
        //        } else {
        //            channel_ = nullptr;
        //            std::cerr << "Failed to connect to " << server_address << ". Scene updates will be ignored." << std::endl;
        //        }
    }
}

} // namespace scene
} // namespace gvs
