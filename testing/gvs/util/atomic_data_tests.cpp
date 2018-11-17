// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Proto Scene Project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/util/atomic_data.hpp"
#include <gmock/gmock.h>
#include <thread>

// Code coverage: Instantiate template class to generate code for all methods.
template class gvs::util::AtomicData<char>;

namespace {

TEST(AtomicDataTests, atomic_data_wait_timeout) {

    struct SharedData {
        bool stop_waiting = false;
        bool update_happened = false;
    };
    gvs::util::AtomicData<SharedData> shared_data;

    // Sleep for 2 seconds then notify the main thread
    std::thread thread([&] {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(2s);
        shared_data.use_safely([](auto& data) { data.stop_waiting = true; });
        shared_data.notify_one();
    });

    // Wait for at most 100ms and exit with false if the timeout is reached
    ASSERT_FALSE(shared_data.wait_to_use_safely(100u,
                                                [](auto& data) { return data.stop_waiting; },
                                                [](auto& data) { data.update_happened = true; }));

    // Since the timeout of 100ms was reached the update should not have happened
    shared_data.use_safely([](const auto& data) { ASSERT_FALSE(data.update_happened); });

    // Wait again up to 5 seconds (the other thread should wake up and notify us within that time)
    ASSERT_TRUE(shared_data.wait_to_use_safely(5000u,
                                               [](auto& data) { return data.stop_waiting; },
                                               [](auto& data) { data.update_happened = true; }));

    thread.join();

    // The update should happen because we were notified before reaching the second timeout
    ASSERT_TRUE(shared_data.unsafe_data().update_happened);
}

TEST(AtomicDataTests, atomic_data_notify_all) {

    struct SharedData {
        bool write_away = false;
        int num_threads = 0;
    };
    gvs::util::AtomicData<SharedData> shared_data;

    std::array<std::thread, 500> threads;

    // Wait for 'notify_all' from the main thread then safely increment 'num_threads' once per thread
    for (auto& thread : threads) {
        thread = std::thread([&] {
            shared_data.wait_to_use_safely([](const auto& data) { return data.write_away; },
                                           [](auto& data) { ++data.num_threads; });
        });
    }

    // notify all the threads at once
    shared_data.use_safely([](auto& data) { data.write_away = true; });
    shared_data.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    // 'num_threads' should exactly equal the amount of threads we created
    ASSERT_EQ(shared_data.unsafe_data().num_threads, threads.size());
}

template <typename T>
class AtomicDataTests : public ::testing::Test {};

typedef ::testing::Types<short, int, unsigned, float, double> Types;
TYPED_TEST_CASE(AtomicDataTests, Types, );

TYPED_TEST(AtomicDataTests, interleaved_atomic_data) {

    struct SharedData {
        TypeParam current_number = 0;
        std::vector<TypeParam> all_data = {};
        std::vector<TypeParam> odds = {};
        std::vector<TypeParam> even = {};
        bool writing_odds = false;
    };

    constexpr short max_number = 9;

    // Use AtomicData structure to update data from two threads
    gvs::util::AtomicData<SharedData> shared_data;

    // Write odd numbers
    std::thread thread([&] {
        bool stop_loop;
        do {
            shared_data.wait_to_use_safely([](const SharedData& data) { return data.writing_odds; },
                                           [&](SharedData& data) {
                                               data.all_data.emplace_back(data.current_number++);
                                               data.odds.emplace_back(data.all_data.back());
                                               stop_loop = data.current_number >= max_number;
                                               data.writing_odds = false;
                                           });
            shared_data.notify_one();
        } while (not stop_loop);
    });

    // Write even numbers
    bool stop_loop;
    do {
        shared_data.wait_to_use_safely([](const SharedData& data) { return not data.writing_odds; },
                                       [&](SharedData& data) {
                                           data.all_data.emplace_back(data.current_number++);
                                           data.even.emplace_back(data.all_data.back());
                                           stop_loop = data.current_number >= max_number;
                                           data.writing_odds = true;
                                       });
        shared_data.notify_one();
    } while (not stop_loop);

    thread.join();

    // shared_data should be correctly ordered despite being modified by separate threads
    ASSERT_EQ(shared_data.unsafe_data().current_number, max_number + 1);
    EXPECT_THAT(shared_data.unsafe_data().even, ::testing::ContainerEq(std::vector<TypeParam>{0, 2, 4, 6, 8}));
    EXPECT_THAT(shared_data.unsafe_data().odds, ::testing::ContainerEq(std::vector<TypeParam>{1, 3, 5, 7, 9}));
    EXPECT_THAT(shared_data.unsafe_data().all_data,
                ::testing::ContainerEq(std::vector<TypeParam>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
    ASSERT_EQ(shared_data.unsafe_data().writing_odds, false);
}

} // namespace
