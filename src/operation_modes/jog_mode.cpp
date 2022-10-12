#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::startJogging() {
    disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(updateJoystickPosition()));
    connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(updateJoystickPosition()));

    ui_->base_r->setDisabled(runningProgram_);
    ui_->increment_1->setDisabled(runningProgram_);
    ui_->a2_r->setDisabled(runningProgram_);
    ui_->increment_2->setDisabled(runningProgram_);
    ui_->a3_r->setDisabled(runningProgram_);
    ui_->increment_3->setDisabled(runningProgram_);
    ui_->a4_r->setDisabled(runningProgram_);
    ui_->increment_4->setDisabled(runningProgram_);
    ui_->a5_r->setDisabled(runningProgram_);
    ui_->increment_5->setDisabled(runningProgram_);
    ui_->grip_r->setDisabled(runningProgram_);
    ui_->increment_6->setDisabled(runningProgram_);
}

void MainWindow::updateJoystickPosition() {
    //float32_t* angles = dev.servoReadall();
    /*float32_t angles[] = 
    {
        dev.angles[0] - joystick->axisLeftX() * 0.05,
        dev.angles[1] - joystick->axisLeftY() * 0.05,
        dev.angles[2] - joystick->axisRightX() * 0.05,
        dev.angles[3] - joystick->axisRightY() * 0.05,
        dev.angles[4] - joystick->buttonLeft() * 0.05,
        dev.angles[4] + joystick->buttonRight() * 0.05,
        dev.angles[5] - round(axes[5] * 100) * 0.1
    };

    for(int i = 0 ; i < 6; i ++)
        std::cout << angles[i] << " ";
    std::cout << '\n';

    dev.servoWrite6(angles, 1000);*/
}
