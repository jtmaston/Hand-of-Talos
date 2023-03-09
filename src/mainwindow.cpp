#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui_(new Ui::MainWindow) {
    setupApplication();
    initCamera();

    running_ = true;
    dev_.toggleTorque(true);
}


MainWindow::~MainWindow() {
    running_ = false;

    quirc_destroy(decoder_);



    camThread_.waitForFinished();
    learnThread_.waitForFinished();
    joyThread_.waitForFinished();
    while(progThread_.isRunning())
    {
        interpreterLock_.unlock();
        progThread_.cancel();
        progThread_.waitForFinished();
    }

    camera_->release();

    delete joystick_;
    delete ui_;
}











