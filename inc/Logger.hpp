#ifndef ROBOTDASHBOARD_LOGGER_HPP
#define ROBOTDASHBOARD_LOGGER_HPP

#include <iostream>
#include <string>

namespace Logger
{
    void error(const std::string& message);
    void warning(const std::string& message);
    void info(const std::string& message);
}

#endif