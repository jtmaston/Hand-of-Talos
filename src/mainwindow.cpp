#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "Logger.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    instructionQueue_ = new std::vector<Instruction>;


    ui->setupUi(this);
    setCameraBarVisibility(HIDDEN);
    setLearnBarVisibility(HIDDEN);

    initSignals();
    initPeripherals();
    initDevice();

    applicationIsRunning_ = true;
    waitForMoveComplete_ = true;
    dev_.timeFactor = 1000;
    progThread_ = QtConcurrent::run(this, &MainWindow::rasmInterpreter, dev_.homePosition_, instructionQueue_, &applicationIsRunning_);
    watchdogThread_ = QtConcurrent::run(this, &MainWindow::runCheckCollision);
    
    dev_.toggleTorque(true);
}

MainWindow::~MainWindow()
{
    applicationIsRunning_ = false;
    antiFreewheel_.unlock();
    cameraThread_.waitForFinished();
    progThread_.waitForFinished();

    uint8_t cmd[] = {0x07, 0};
    write(dev_.led_bus, cmd, 2);
    quirc_destroy(qrDecoder_);
    delete instructionQueue_;     // FIXME: placement of this here is dangerous and will result in segfault! Garbage code!
    delete joystick_;
    delete ui;
}