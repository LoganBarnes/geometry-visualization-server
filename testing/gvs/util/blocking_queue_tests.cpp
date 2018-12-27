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
#include "gvs/util/blocking_queue.hpp"

#include <gmock/gmock.h>

#include <thread>

// Code coverage: Instantiate template class to generate code for all methods.
template class gvs::util::BlockingQueue<char>;

namespace {

test(BlockingQueueTests, empty_is_empty) {
    gvs::util::BlockingQueue<char> bq;
    ASSERT_EQ(bq.size(), 0);
    ASSERT_TRUE(bq.empty());

    bq.emplace_back('$');

    ASSERT_EQ(bq.size(), 1);
    ASSERT_FALSE(bq.empty());
}

test(BlockingQueueTests, pop_all_but_most_recent) {
    gvs::util::BlockingQueue<std::string> strings;

    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";

    for (char c : alphabet) {
        strings.emplace_back(1, c); // emplace_back for specific std::string constructor
    }

    ASSERT_EQ(strings.pop_all_but_most_recent(), "z");
    ASSERT_EQ(strings.size(), 1);
}

template <typename T>
class BlockingQueueTests : public ::testing::Test {};

typedef ::testing::Types<int, float, double> Types;
typed_test_case(BlockingQueueTests, Types);

typed_test(BlockingQueueTests, interleaved_data) {

    std::vector<TypeParam> shared_data;

    // Use two blocking queues as control structures between two threads
    gvs::util::BlockingQueue<TypeParam> even_bq;
    gvs::util::BlockingQueue<TypeParam> odds_bq;

    // Wait for the next even number from 'even_bq',
    // Write the number to 'shared_data',
    // Write the next odd number to 'odds_bq'
    std::thread thread([&] {
        for (int i = 1; i < 10; i += 2) {
            TypeParam to_add = even_bq.pop_front();
            shared_data.emplace_back(to_add);
            odds_bq.push_back(i);
        }
    });

    // Write the next even number to 'even_bq'
    // Wait for the next odd number from 'odds_bq',
    // Write the number to 'shared_data',
    for (int i = 0; i < 10; i += 2) {
        even_bq.push_back(i);
        TypeParam to_add = odds_bq.pop_front();
        shared_data.emplace_back(to_add);
    }

    thread.join();

    // All the data should be processed by now
    ASSERT_TRUE(even_bq.empty());
    ASSERT_TRUE(odds_bq.empty());

    // shared_data should be correctly ordered despite being modified by separate threads
    ASSERT_EQ(shared_data, (std::vector<TypeParam>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

} // namespace
