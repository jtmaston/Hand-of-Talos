#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::initDevice()
{
    dev_.angles.clear();
    for (int i = 0; i < 6; i++)
        dev_.angles.push_back(0);

    uiSliderArray_[0] = ui->base_r;
    uiSliderArray_[1] = ui->a2_r;
    uiSliderArray_[2] = ui->a3_r;
    uiSliderArray_[3] = ui->a4_r;
    uiSliderArray_[4] = ui->a5_r;
    uiSliderArray_[5] = ui->grip_r;
    dev_.timeFactor = 1000;

    dev_.go_home();
    crappyDelay(dev_.timeFactor);
}

void MainWindow::runGetCurrentPosition()
{
    dev_.getCurrentPosition();
}

void MainWindow::runCheckCollision()
{
    dev_.checkCollision();
}