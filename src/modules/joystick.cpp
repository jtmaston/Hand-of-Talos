//
// Created by aanas on 12/12/22.
//
#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

bool previously_connected = false;
auto MainWindow::joystickHotplugDetect() -> bool
{

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty())
    {
        if(previously_connected)
            Logger::info("Joystick disconnected!");

        if (joystick_ != nullptr)
        {
            delete joystick_;
            joystick_ = nullptr;
        }
        return false;
    }

    if (joystick_ != nullptr)
        return true;

    //std::cout << "[ INFO ] Joystick connected\n";
    Logger::info("Joystick connected!");
    previously_connected = true;

    joystick_ = new QGamepad(*gamepads.begin(), this);

    return true;
}
