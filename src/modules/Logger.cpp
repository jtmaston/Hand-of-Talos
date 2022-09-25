#include "Logger.hpp"

void Logger::Error(std::string message)
{
    std::cout << "\033[;31m"
              << "[" << "ERR" << "]" <<"\033[0m: "
              << message << '\n';
}

void Logger::Warning(std::string message)
{
    std::cout << "\033[;33m"
              << "[" << "WARN" << "]" <<"\033[0m: "
              << message << '\n';
}

void Logger::Info(std::string message)
{
    std::cout << "\033[;34m"
              << "[" <<"INFO" << "]" <<"\033[0m: "
              << message << '\n';
              
}