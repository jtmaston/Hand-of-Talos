#include "mainwindow.hpp"
#include "ui_mainwindow.h"

/*void MainWindow::jog()
{
    switch (following_program)
    {
    case 0:
        connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_stick()));
        time_mod = 300;
        dev.toggleTorque(true);
        if (learning)
            connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command())); // control from the axis is also updated ever 100ms

        break;
    case 1:
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(update_stick()));
        time_mod = 1000;
        break;
    }
    following_program = !following_program;
    toggle_jog();
    //std::cout << following_program << '\n';
    //std::cout.flush();
}*/

void MainWindow::jog()
{
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(update_stick()));
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_stick()));

    toggle_jog();
}

void MainWindow::update_stick()
{
    //float32_t* angles = dev.servo_readall();
    float32_t angles[] = 
    {
        ui->base_r->value() - joystick->axisLeftX() * 100 * 0.05,
        ui->a2_r->value() - joystick->axisLeftY() * 100 * 0.05,
        ui->a3_r->value() - joystick->axisRightX() * 100 * 0.05,
        ui->a4_r->value() - joystick->axisRightY() * 100 * 0.05,
        ui->a5_r->value() - joystick->buttonLeft() * 0.05,
        ui->a5_r->value() + joystick->buttonRight() * 0.05,
        ui->grip_r->value() - round(axes[5] * 100) * 0.1
    };

    for(int i = 0 ; i < 6; i ++)
        std::cout << angles[i] << " ";
    std::cout << '\n';

    //dev.servo_write6(angles, 1000);
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