#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::jog()
{
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(update_stick()));
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_stick()));

    toggle_jog();
}

void MainWindow::update_stick()
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

void MainWindow::toggle_jog()
{
    ui->base_r->setDisabled(following_program);
    ui->increment_1->setDisabled(following_program);
    ui->a2_r->setDisabled(following_program);
    ui->increment_2->setDisabled(following_program);
    ui->a3_r->setDisabled(following_program);
    ui->increment_3->setDisabled(following_program);
    ui->a4_r->setDisabled(following_program);
    ui->increment_4->setDisabled(following_program);
    ui->a5_r->setDisabled(following_program);
    ui->increment_5->setDisabled(following_program);
    ui->grip_r->setDisabled(following_program);
    ui->increment_6->setDisabled(following_program);
}