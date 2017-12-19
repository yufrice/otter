#include <logger/logger.hpp>

namespace otter {
    namespace logger {

        bool Logger::debugFlag = false;
        std::chrono::time_point<std::chrono::system_clock> Logger::startTime =
            std::chrono::system_clock::now();

    }// logger
}// otter