#pragma once

#include <chrono>
#include <iostream>

class progress_bar 
{
private:
    unsigned int ticks = 0;

    const size_t total_ticks;
    const size_t bar_width;
    const char complete_char = '=';
    const char incomplete_char = ' ';
    const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

public:
    progress_bar(size_t total, size_t width, char complete, char incomplete) :
        total_ticks{ total }, bar_width{ width }, complete_char{ complete }, incomplete_char{ incomplete } {}

    progress_bar(size_t total, size_t width) : total_ticks{ total }, bar_width{ width } {}

    unsigned int operator++() { return ++ticks; }

    void display() const
    {
        auto progress = static_cast<float>(ticks) / total_ticks;
        auto pos = static_cast<size_t>(bar_width * progress);

        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count();

        std::cout << "[";

        for (size_t i = 0; i < bar_width; ++i)
        {
            if (i < pos) std::cout << complete_char;
            else if (i == pos) std::cout << ">";
            else std::cout << incomplete_char;
        }

        std::cout << "] " << static_cast<int>(progress * 100.0) << "% " << static_cast<float>(time_elapsed) / 1000.0 << "s\r";
        std::cout.flush();
    }

    void done() const
    {
        display();
        std::cout << std::endl;
    }
};