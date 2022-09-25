#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>

namespace Logger
{
    void Error(std::string);
    void Warning(std::string);
    void Info(std::string);
}

#endif