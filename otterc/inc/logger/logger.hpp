#ifndef _OTTER_LOGGER_LOGGER_HPP_
#define _OTTER_LOGGER_LOGGER_HPP_

#include <iostream>
#include <string>
#include <chrono>

namespace otter {
    namespace logger {

        class Logger {
           private:
            static bool debugFlag;
            static std::chrono::time_point<std::chrono::system_clock> startTime;

            auto time() {
                auto dur = std::chrono::system_clock::now() - startTime;
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                           dur)
                    .count();
            }

           public:
            Logger() { startTime = std::chrono::system_clock::now(); }

            Logger(bool flag) {
                this->debugFlag = flag;
                startTime       = std::chrono::system_clock::now();
            }

            void _assert(std::string str) {
                if (this->debugFlag) {
                    std::cout << time() << " : " << str << std::endl;
                }
            }
        };

        bool Logger::debugFlag = false;
        std::chrono::time_point<std::chrono::system_clock> Logger::startTime =
            std::chrono::system_clock::now();
    }  // namespace logger
}  // namespace otter

#endif
