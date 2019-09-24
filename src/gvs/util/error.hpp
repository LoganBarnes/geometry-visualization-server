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

// standard
#include <string>

/**
 * @brief Macro used to auto-fill line and file information when creating an error.
 *
 * Example usage:
 *
 * ```
 *     gvs::util::Error do_thing_or_give_errors() {
 *
 *         ... do stuff ...
 *
 *         if (bad_thing) {
 *             return MAKE_ERROR("Bad thing happened!");
 *         }
 *         return gvs::util::Error::no_errors();
 *     }
 * ```
 */
#define MAKE_ERROR(msg) gvs::util::Error({__FILE__, __LINE__}, msg)

namespace gvs::util {

struct SourceLocation {
    std::string filename;
    int line_number;

    SourceLocation() = delete;
    SourceLocation(std::string file, int line);
};

/**
 * @brief A simple class used to pass error messages around.
 *
 * This class can be used as a base class for more complicated error types.
 */
class Error {
public:
    Error() = delete;
    explicit Error(const SourceLocation& source_location, std::string error_message);
    virtual ~Error();

    // default copy
    Error(const Error&) = default;
    Error& operator=(const Error&) = default;

    // default move
    Error(Error&&) noexcept = default;
    Error& operator=(Error&&) noexcept = default;

    const std::string& error_message() const;
    const std::string& debug_error_message() const;

    bool operator==(const Error& other) const;

private:
    std::string error_message_; ///< The error message
    std::string debug_message_; ///< Error message with file and line number "[file:line] error message"
};

} // namespace gvs::util
