#pragma once

#include <string>
#include <iostream>

namespace ml
{
    namespace utils
    {
        class Logger
        {

        public:
            static Logger& getInstance()
            {
                static Logger instance;
                return instance;
            }

            static void Info(std::string&& component, std::string&& message) noexcept
            {
                std::cout << '[' << component << ']' << " INF: " << message << '\n' << std::flush;
            }

            static void Warning(std::string&& component, std::string&& message) noexcept
            {
                std::cout << '[' << component << ']' << " WRN: " << message << '\n' << std::flush;
            }

            static void Error(std::string&& component, std::string&& message) noexcept
            {
                std::cout << '[' << component << ']' << " ERR: " << message << '\n' << std::flush;
            }

        private:
            Logger() = default;
            ~Logger() = default;
            Logger(const Logger&) = delete;
            Logger& operator=(const Logger&) = delete;
        };
    }
}