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
#include "error.hpp"

// generated
#include <gvs/gvs_paths.hpp>

// external
#include <doctest/doctest.h>

namespace gvs::util {

SourceLocation::SourceLocation(std::string file, int line) : filename(std::move(file)), line_number(line) {}

namespace {

/**
 * @brief Get the file string relative to the project root dir.
 */
std::string get_relative_path_string(const std::string& path) {
    auto abs_path_start = path.find(paths::project_root());

    // Remove the project path if it exists in `path`
    if (abs_path_start != std::string::npos) {
        return path.substr(abs_path_start + paths::project_root().size());
    }

    return path;
}

TEST_CASE("[util] get relative path string") {
    CHECK(get_relative_path_string("/no/project/root") == "/no/project/root");
    CHECK(get_relative_path_string(paths::project_root() + "with/project/root") == "with/project/root");
}

/**
 * @brief Prepend the file and line number to the message if they were specified.
 */
std::string create_debug_message(const SourceLocation& source_location, const std::string& error_message) {
    std::string result = "[";

    if (!source_location.filename.empty()) {
        result += get_relative_path_string(source_location.filename);
    }

    if (source_location.line_number >= 0) {
        result += ':' + std::to_string(source_location.line_number);
    }

    result += "] " + error_message;
    return result;
}

} // namespace

Error::~Error() = default;

Error::Error(const SourceLocation& source_location, std::string error_message)
    : error_message_(std::move(error_message)), debug_message_(create_debug_message(source_location, error_message_)) {}

const std::string& Error::error_message() const {
    return error_message_;
}

const std::string& Error::debug_error_message() const {
    return debug_message_;
}

bool Error::operator==(const Error& other) const {
    return debug_message_ == other.debug_message_;
}

TEST_CASE("[util] check error helpers") {
    CHECK(MAKE_ERROR("Error message").error_message() == "Error message");
    CHECK(MAKE_ERROR("Error message").debug_error_message()
          == "[src/gvs/util/error.cpp:" + std::to_string(__LINE__) + "] Error message");
    CHECK(MAKE_ERROR("blarg") == MAKE_ERROR("blarg"));
}

} // namespace gvs::util