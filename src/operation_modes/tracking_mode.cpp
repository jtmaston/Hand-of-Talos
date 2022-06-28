#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::start_follow_red()
{
    switch (dir)
    {
    case 0:
    {
        dir = 1;
        connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
        break;
    }
    }
    dir = 1;

    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 0};

    write(dev.led_bus, mode, 2);
    write(dev.led_bus, speed, 2);
    write(dev.led_bus, color, 2);
}

void MainWindow::start_follow_green()
{
    switch (dir)
    {
    case 0:
    {
        dir = 2;
        connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
        break;
    }
    }
    dir = 2;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 1};

    write(dev.led_bus, mode, 2);
    write(dev.led_bus, speed, 2);
    write(dev.led_bus, color, 2);
}

void MainWindow::start_follow_blue()
{
    switch (dir)
    {
    case 0:
    {
        dir = 3;
        connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
        break;
    }
    }
    dir = 3;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 2};

    write(dev.led_bus, mode, 2);
    write(dev.led_bus, speed, 2);
    write(dev.led_bus, color, 2);
}

void MainWindow::stop_follow()
{
    disconnect(Scheduler_500ms, SIGNAL(timeout()), this, SLOT(follow()));
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
}

void MainWindow::follow()
{
    int x_center = 640 / 2;
    int y_center = 480 / 2;

    switch (dir)
    {
    case 1:
    {
        x_center = red.x + red.width / 2;
        y_center = red.y + red.y / 2;
        break;
    }
    case 2:
    {
        x_center = green.x + green.width / 2;
        y_center = green.y + green.y / 2;
        break;
    }
    case 3:
    {
        x_center = blue.x + blue.width / 2;
        y_center = blue.y + blue.y / 2;
        break;
    }

    }
    float32_t alpha = (asin((320 - x_center) / ((22) * pixMod)) * 180) / __PI__;
    float32_t beta = (asin((240 - y_center) / ((36) * pixMod)) * 180) / __PI__;

    ui->base_r->setValue(ui->base_r->value() + round(alpha));
    ui->a4_r->setValue(ui->a4_r->value() - round(beta));

    float32_t angles[6];
    angles[0] = ui->increment_1->value() + dev.home_position[0]; // need to adjust with 90
    angles[1] = ui->increment_2->value() + dev.home_position[1];
    angles[2] = ui->increment_3->value() + dev.home_position[2];
    angles[3] = ui->increment_4->value() + dev.home_position[3];
    angles[4] = ui->increment_5->value() + dev.home_position[4];
    angles[5] = ui->increment_6->value() + dev.home_position[5];

    ui->base_r->setValue(static_cast<int>(ui->increment_1->value()));
    ui->a2_r->setValue(static_cast<int>(ui->increment_2->value()));
    ui->a3_r->setValue(static_cast<int>(ui->increment_3->value()));
    ui->a4_r->setValue(static_cast<int>(ui->increment_4->value()));
    ui->a5_r->setValue(static_cast<int>(ui->increment_5->value()));
    ui->grip_r->setValue(static_cast<int>(ui->increment_6->value()));

    //dev.servo_write6(angles, 450);        FIXME:
}