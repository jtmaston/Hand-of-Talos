//
// Created by aanas on 12/19/22.
//

#include "Logger.hpp"

void Logger::error(const std::string &message) {
    std::cout << "\033[;31m"
              << "[" << "ERR" << "]" << "\033[0m: "
              << message << '\n';
    throw std::runtime_error(message.c_str());
}

void Logger::warning(const std::string &message) {
    std::cout << "\033[;33m"
              << "[" << "WARN" << "]" << "\033[0m: "
              << message << '\n';
}

void Logger::info(const std::string &message) {
#ifdef LOGLEVEL_INFO
    std::cout << "\033[;34m"
              << "[" << "INFO" << "]" << "\033[0m: "
              << message << '\n';
#endif
}