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
            Logger() {}

            Logger(bool flag) {
                this->debugFlag = flag;
            }

            void _assert(std::string str) {
                if (this->debugFlag) {
                    std::cout << time() << " : " << str << std::endl;
                }
            }
        };

    }  // namespace logger
}  // namespace otter

#endif
