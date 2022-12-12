//
// Created by aanas on 12/12/22.
//
#include "inc/mainwindow.h"
#include "ui_mainwindow.h"


auto MainWindow::joystickHotplugDetect() -> bool
{

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty())
    {
        ui_->jog_btn->setEnabled(false);
        //std::cout << "[ WARN ] No gamepad detected.\n";

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

    joystick_ = new QGamepad(*gamepads.begin(), this);

    return true;
}
