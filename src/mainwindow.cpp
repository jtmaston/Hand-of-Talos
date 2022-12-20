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
    camThread_.waitForFinished();
    joyThread_.waitForFinished();

    std::array<uint8_t, 2> cmd = {0x07, 0};
    write(dev_.led_bus, cmd.data(), 2);

    quirc_destroy(decoder_);

    interpreterLock_.unlock();

    delete joystick_;
    delete ui_;
}











