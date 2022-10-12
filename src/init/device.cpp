#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::initDevice()
{
    dev_.currentPosition_.clear();
    for (int i = 0; i < 6; i++)
        dev_.currentPosition_.push_back(0);


    dev_.timeFactor_ = 1000;

    dev_.goHome();
    crappyDelay(dev_.timeFactor_);
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
        // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        //std::cout << "Took " << duration.count() << '\n';
        
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
    }
    
}