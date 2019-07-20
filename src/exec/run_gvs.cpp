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
#include "gvs/vis-client/vis_client.hpp"

#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <mutex>
#include <unistd.h>

namespace {

// Allows the main thread to block until someone sends SIGINT
bool exit_server = false;
std::mutex mtx;
std::condition_variable block_until_signal;

void signal_handler(int /*signal*/) {
    std::lock_guard lock(mtx);
    std::cout << "\nCaught signal!" << std::endl;
    exit_server = true;
    // Let the main thread continue
    block_until_signal.notify_one();
}

} // namespace

int main(int argc, char** argv) {
    std::signal(SIGINT, signal_handler);

    std::string host_address = "0.0.0.0:50055";
    bool client_only = false;
    bool server_only = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        constexpr auto host_flag = "--host=";
        if (arg.rfind(host_flag, 0) == 0) {
            host_address = arg.substr(std::string(host_flag).size());
        }

        constexpr auto client_only_flag = "-c";
        if (arg.rfind(client_only_flag, 0) == 0) {
            client_only = true;
            server_only = false;
        }

        constexpr auto server_only_flag = "-s";
        if (arg.rfind(server_only_flag, 0) == 0) {
            server_only = true;
            client_only = false;
        }
    }

    std::unique_ptr<gvs::server::SceneServer> server;
    if (!client_only) {
        server = std::make_unique<gvs::server::SceneServer>(host_address);
        std::cout << "Server running at '" << host_address << "'" << std::endl;
    }

    if (!server_only) {
        gvs::vis::VisClient app(host_address, {argc, argv});
        return app.exec();
    }

    std::cout << "'CTRL + C' to quit..." << std::endl;

    // Block the main thread until someone sends SIGINT
    std::unique_lock lock(mtx);
    block_until_signal.wait(lock, [] { return exit_server; });
    std::cout << "Exiting." << std::endl;
    return 0;
}
