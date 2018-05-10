
#include "Timer.hpp"

#if COMPILE_TESTS

#include "gtest/gtest.h"
#include <chrono> // sleeping
#include <thread> // sleeping
TEST(utils, TimerTests)
{
    Timer t("markertest", false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
//    std::cout << t.ellapsed(std::chrono::microseconds(1)) << std::endl;
//    std::cout << t.ellapsed(std::chrono::milliseconds(1)) << std::endl;
    EXPECT_EQ(t.ellapsed(std::chrono::microseconds(1)), true);
    t.mark();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    EXPECT_EQ(t.ellapsed(std::chrono::milliseconds(2)), false);
    t.mark();
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    EXPECT_EQ(t.ellapsed(std::chrono::milliseconds(2)), true);
    t.mark();
    EXPECT_EQ(t.ellapsed(std::chrono::seconds(1)), false);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(t.ellapsed(std::chrono::seconds(1)), true);
}

#endif

