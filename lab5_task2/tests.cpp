#define CATCH_CONFIG_MAIN
//исправить, не лазить в папку оод
#include "../../../../Desktop/ood/catch2/catch.hpp"
#include "BgThreadDispatcher.h"
#include <thread>
#include <chrono>

TEST_CASE("BgThreadDispatcher executes task in background")
{
    BgThreadDispatcher dispatcher;
    bool executed = false;

    dispatcher.Dispatch([&]
        { executed = true; });
    dispatcher.Wait();

    REQUIRE(executed);
}

TEST_CASE("BgThreadDispatcher executes tasks in order")
{
    BgThreadDispatcher dispatcher;
    std::vector<int> results;

    dispatcher.Dispatch([&]
        { results.push_back(1); });
    dispatcher.Dispatch([&]
        { results.push_back(2); });
    dispatcher.Dispatch([&]
        { results.push_back(3); });

    dispatcher.Wait();

    REQUIRE(results == std::vector<int>{1, 2, 3});
}

TEST_CASE("BgThreadDispatcher continues tasks after exception")
{
    BgThreadDispatcher dispatcher;
    bool secondTaskExecuted = false;

    dispatcher.Dispatch([]
        { throw std::runtime_error("Test exception"); });
    dispatcher.Dispatch([&]
        { secondTaskExecuted = true; });

    dispatcher.Wait();

    REQUIRE(secondTaskExecuted);
}

TEST_CASE("BgThreadDispatcher stops accepting tasks after Stop")
{
    BgThreadDispatcher dispatcher;
    bool taskExecuted = false;

    dispatcher.Stop();
    dispatcher.Dispatch([&]
        { taskExecuted = true; });
    dispatcher.Wait();

    REQUIRE_FALSE(taskExecuted);
}

TEST_CASE("BgThreadDispatcher stops background thread")
{
    BgThreadDispatcher dispatcher;
    bool taskExecuted = false;
    dispatcher.Stop();

    dispatcher.Dispatch([&]
        { taskExecuted = true; });

    dispatcher.Wait();
    REQUIRE(!taskExecuted);
}

TEST_CASE("BgThreadDispatcher Wait waits for all tasks")
{
    BgThreadDispatcher dispatcher;
    bool taskCompleted = false;

    dispatcher.Dispatch([&]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            taskCompleted = true;
        });

    dispatcher.Wait();

    REQUIRE(taskCompleted);
}
