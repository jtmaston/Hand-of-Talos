#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::init_device()
{
    dev.angles.clear();
    for(int i = 0 ; i < 6; i++)
        dev.angles.push_back(0);

    slider_array[0] = ui->base_r;
    slider_array[1] = ui->a2_r;
    slider_array[2] = ui->a3_r;
    slider_array[3] = ui->a4_r;
    slider_array[4] = ui->a5_r;
    slider_array[5] = ui->grip_r;
    
    //home_position = { 90, 90, 90, 0, 90, 0 };
    dev.home_position = {90, 90, 90, 180, 90, 90};

}