#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::startJogging()
{
    disconnect(Scheduler_100ms_, SIGNAL(timeout()), this, SLOT(updateJoystickPosition()));
    connect(Scheduler_100ms_, SIGNAL(timeout()), SLOT(updateJoystickPosition()));

    ui->base_r->setDisabled(runningProgram_);
    ui->increment_1->setDisabled(runningProgram_);
    ui->a2_r->setDisabled(runningProgram_);
    ui->increment_2->setDisabled(runningProgram_);
    ui->a3_r->setDisabled(runningProgram_);
    ui->increment_3->setDisabled(runningProgram_);
    ui->a4_r->setDisabled(runningProgram_);
    ui->increment_4->setDisabled(runningProgram_);
    ui->a5_r->setDisabled(runningProgram_);
    ui->increment_5->setDisabled(runningProgram_);
    ui->grip_r->setDisabled(runningProgram_);
    ui->increment_6->setDisabled(runningProgram_);
}

void MainWindow::updateJoystickPosition()
{
    //float32_t* angles = dev.servo_readall();
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

    dev.servo_write6(angles, 1000);*/
}
