/*
        Copyright(C) 2025 Fordans
                        This source follows the GPL licence
                        See https://www.gnu.org/licenses/gpl-3.0.html for details
*/

#ifndef FDS_FUNCTIONAL_TIMER_H
#define FDS_FUNCTIONAL_TIMER_H

#include <thread>
#include <chrono>
#include <functional>
#include <optional>

namespace fds
{
    class FunctionalTimer
    {
    public:
        FunctionalTimer() = default;
        ~FunctionalTimer() = default;

        // Wait for a specified number of milliseconds and then execute a callback
        void wait(int milliseconds, std::function<void()> callback)
        {
            std::thread([milliseconds, callback]()
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
                    if (callback)
                    {
                        callback();
                    }
                }).detach();
        }

        // Loop with rounds, 0 means infinite loop
        void loop(int milliseconds, std::function<void()> callback, int rounds, std::optional<bool> execute_immediately = std::nullopt)
        {
            std::thread([milliseconds, callback, rounds, execute_immediately]()
                {
                    int current_round = 0;
                    bool should_execute_immediately = execute_immediately.value_or(false);

                    if (should_execute_immediately)
                    {
                        if (callback)
                        {
                            callback();
                        }
                        current_round++;
                    }

                    while (rounds == 0 || current_round < rounds)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
                        if (callback)
                        {
                            callback();
                        }
                        current_round++;
                    }
                }).detach();
        }

        // Loop with condition
        void loop(int milliseconds, std::function<void()> callback, std::function<bool()> condition, std::optional<bool> execute_immediately = std::nullopt)
        {
            std::thread([milliseconds, callback, condition, execute_immediately]()
                {
                    bool should_execute_immediately = execute_immediately.value_or(false);

                    if (should_execute_immediately)
                    {
                        if (callback)
                        {
                            callback();
                        }
                    }

                    while (condition())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
                        if (callback)
                        {
                            callback();
                        }
                    }
                }).detach();
        }

        FunctionalTimer(const FunctionalTimer &) = delete;
        FunctionalTimer &operator=(const FunctionalTimer &) = delete;
        FunctionalTimer(FunctionalTimer &&) = delete;
        FunctionalTimer &operator=(FunctionalTimer &&) = delete;
    };
}

#endif // FDS_FUNCTIONAL_TIMER_H