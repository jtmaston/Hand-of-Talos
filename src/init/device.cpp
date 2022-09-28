#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::initDevice()
{
    dev_.currentPosition_.clear();
    for (int i = 0; i < 6; i++)
        dev_.currentPosition_.push_back(0);

    uiSliderArray_[0] = ui->base_r;
    uiSliderArray_[1] = ui->a2_r;
    uiSliderArray_[2] = ui->a3_r;
    uiSliderArray_[3] = ui->a4_r;
    uiSliderArray_[4] = ui->a5_r;
    uiSliderArray_[5] = ui->grip_r;
    dev_.timeFactor = 1000;

    dev_.goHome();
    crappyDelay(dev_.timeFactor);
}

void MainWindow::runGetCurrentPosition()                    //      
{
    dev_.getCurrentPosition();
}

void MainWindow::runCheckCollision()
{
    while(applicationIsRunning_)
    {
        auto start = std::chrono::high_resolution_clock::now();
        dev_.checkCollision();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Took " << duration.count() << '\n';
        crappyDelay(10);
        
    }
    
}