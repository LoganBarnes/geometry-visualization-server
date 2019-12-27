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
#include "gvs/display/log_params.hpp"
#include "gvs/display/scene_info_sender.hpp"
#include "gvs/display/send.hpp"

// standard
#include <iostream>

namespace gvs::log {

/// \brief Checks for errors from a GeometryItemStream and throws a std::runtime_error if they exist
#define CHECK_WITH_THROW(stream)                                                                                       \
    if (not stream.success()) {                                                                                        \
        throw std::runtime_error("Error (stream " + stream.id() + "): " + stream.error_message());                     \
    }

/// \brief Checks for errors from a GeometryItemStream and prints them to std::cerr if they exist
#define CHECK_WITH_PRINT(stream)                                                                                       \
    if (not stream.success()) {                                                                                        \
        std::cerr << "Error (stream " << stream.id() << "): " << stream.error_message() << std::endl;                  \
    }

/// \brief A single item stream
class GeometryItemStream {
public:
    explicit GeometryItemStream(std::string id, SceneInfoSender& sender);

    /// \brief Sends all the data currently stored in this stream
    void send_current_data(SendType type);

    /// \brief Update the current state of this stream
    ///
    ///     ```cpp
    ///     // Create the scene
    ///     gvs::log::GeometryLogger scene("localhost:50055", 3s);
    ///
    ///     // create a stream
    ///     gvs::log::GeometryItemStream stream = scene.item_stream();
    ///
    ///     // modify the stream
    ///     stream << gvs::positions_3d({}) << gvs::line_strip({}));
    ///
    ///     // send the stream contents to the server
    ///     stream << gvs::send; // or gvs::replace or gvs::append
    ///     ```
    template <typename Functor>
    GeometryItemStream& operator<<(Functor&& functor);

    /// \brief Send the currently stored items in this stream
    ///
    ///     ```cpp
    ///     // Create the scene
    ///     gvs::log::GeometryLogger scene("localhost:50055", 3s);
    ///
    ///     // create a stream
    ///     gvs::log::GeometryItemStream stream = scene.item_stream();
    ///
    ///     // modify the stream
    ///     stream << gvs::positions_3d({}) << gvs::line_strip({}));
    ///
    ///     // send the stream contents to the server
    ///     stream << gvs::send; // or gvs::replace or gvs::append
    ///     ```
    GeometryItemStream& operator<<(GeometryItemStream& (*send_func)(GeometryItemStream&));

    /// \brief Modifies and sends the contents of a stream, updating the server if the stream id does not exist
    ///
    ///     ```cpp
    ///     // Create the scene
    ///     gvs::log::GeometryLogger scene("localhost:50055", 3s);
    ///
    ///     // create a stream
    ///     gvs::log::GeometryItemStream stream = scene.item_stream();
    ///
    ///     // modify and send stream contents to the server
    ///     stream.send(gvs::positions_3d({}), gvs::line_strip({}));
    ///     ```
    template <typename... Functors>
    GeometryItemStream& send(Functors&&... functors);

    /// \brief Modifies and sends the contents of the stream, replacing any server items with the same id
    ///
    ///     ```cpp
    ///     // Create the scene
    ///     gvs::log::GeometryLogger scene("localhost:50055", 3s);
    ///
    ///     // create a stream
    ///     gvs::log::GeometryItemStream stream = scene.item_stream();
    ///
    ///     // modify and send stream contents to the server
    ///     stream.replace(gvs::positions_3d({}), gvs::line_strip({}));
    ///     ```
    template <typename... Functors>
    GeometryItemStream& replace(Functors&&... functors);

    /// \brief Modifies and sends the contents of a stream, appending geometry to a server item with the same id
    ///
    ///     ```cpp
    ///     // Create the scene
    ///     gvs::log::GeometryLogger scene("localhost:50055", 3s);
    ///
    ///     // create a stream
    ///     gvs::log::GeometryItemStream stream = scene.item_stream();
    ///
    ///     // modify and send stream contents to the server
    ///     stream.append(gvs::positions_3d({}), gvs::line_strip({}));
    ///     ```
    template <typename... Functors>
    GeometryItemStream& append(Functors&&... functors);

    /// \brief Get the stream id
    [[nodiscard]] auto id() const -> std::string const&;

    /// \brief Return true if no errors have occurred while modifying or sending the stream contents
    [[nodiscard]] auto success() const -> bool;

    /// \brief The associated error message if GeometryItemStream::success returns false
    [[nodiscard]] auto error_message() const -> std::string const&;

private:
    const std::string id_; ///< The id of the stream
    SceneItemInfo     info_; ///< The current state of the stream

    SceneInfoSender& sender_; ///< Updates the scene when requested

    std::string error_message_ = ""; ///< error messages for this stream, empty if there are none
};

template <typename Functor>
GeometryItemStream& GeometryItemStream::operator<<(Functor&& functor) {
    std::string error_name = functor(&info_);
    if (not error_name.empty()) {
        throw std::invalid_argument(error_name + " is already set");
    }
    return *this;
}

template <typename... Functors>
GeometryItemStream& GeometryItemStream::send(Functors&&... functors) {
    // iterate over all functors and apply this stream's << operator to them
    int dummy[] = {(this->operator<<(std::forward<Functors>(functors)), 0)...};
    (void)dummy; // ignore this necessary but unused variable

    this->send_current_data(SendType::Safe);
    return *this;
}

template <typename... Functors>
GeometryItemStream& GeometryItemStream::replace(Functors&&... functors) {
    // iterate over all functors and apply this stream's << operator to them
    int dummy[] = {(this->operator<<(std::forward<Functors>(functors)), 0)...};
    (void)dummy; // ignore this necessary but unused variable

    this->send_current_data(SendType::Replace);
    return *this;
}

template <typename... Functors>
GeometryItemStream& GeometryItemStream::append(Functors&&... functors) {
    // iterate over all functors and apply this stream's << operator to them
    int dummy[] = {(this->operator<<(std::forward<Functors>(functors)), 0)...};
    (void)dummy; // ignore this necessary but unused variable

    this->send_current_data(SendType::Append);
    return *this;
}

} // namespace gvs::log
