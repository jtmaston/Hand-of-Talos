//
// Created by aanas on 12/19/22.
//

#ifndef ROBOTDASHBOARD_LOGGER_HPP
#define ROBOTDASHBOARD_LOGGER_HPP

#include <iostream>
#include <string>
#include <exception>

#define LOGLEVEL_INFO

namespace Logger
{
    void error(const std::string& message);
    void warning(const std::string& message);
    void info(const std::string& message);
}



#endif //ROBOTDASHBOARD_LOGGER_HPP
