/*
        Copyright(C) 2025 Fordans
                        This source follows the GPL licence
						See https://www.gnu.org/licenses/gpl-3.0.html for details
*/

#pragma once

#include <chrono>
#include <thread>

class FDS_Timer
{
public:
    FDS_Timer() : m_startTime(std::chrono::high_resolution_clock::now()) {}

    double peek() const
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(current_time - m_startTime);
        return duration.count();
    }

    void reset()
    {
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    // Unit is Seconds
    void delay(double seconds)
    {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};
