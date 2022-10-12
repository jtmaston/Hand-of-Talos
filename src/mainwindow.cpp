#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "Logger.hpp"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui_(new Ui::MainWindow) {

    applicationIsRunning_ = true;
    waitForMoveComplete_ = true;

    instructionQueue_ = new std::vector<Instruction>;
    qrDecoder_ = nullptr;

    gameplayManager_ = nullptr;

    ui_->setupUi(this);
    setCameraBarVisibility(HIDDEN);
    setLearnBarVisibility(HIDDEN);

    initSignals();
    initPeripherals();
    initDevice();

    dev_.timeFactor_ = 1000;
    progThread_ = QtConcurrent::run(this, &MainWindow::rasmInterpreter, dev_.homePosition_, instructionQueue_,
                                    &applicationIsRunning_);
    watchdogThread_ = QtConcurrent::run(this, &MainWindow::runCheckCollision);

    uiSliderArray_[0] = ui_->base_r;
    uiSliderArray_[1] = ui_->a2_r;
    uiSliderArray_[2] = ui_->a3_r;
    uiSliderArray_[3] = ui_->a4_r;
    uiSliderArray_[4] = ui_->a5_r;
    uiSliderArray_[5] = ui_->grip_r;

    dev_.toggleTorque(true);
}

void MainWindow::testProcedure() {
    for(int i = 0 ; i < 4; i++)
    {
        ui_->increment_1->setValue(120);
        moveToPosition();
        crappyDelay(3500);


        ui_->increment_1->setValue(0);
        moveToPosition();
        crappyDelay(3500);
    }

}

MainWindow::~MainWindow() {
    applicationIsRunning_ = false;
    antiFreewheel_.unlock();
    cameraThread_.waitForFinished();
    progThread_.waitForFinished();

    std::array<uint8_t, 2> cmd = {0x07, 0};
    write(dev_.ledBus_, cmd.data(), 2);
    quirc_destroy(qrDecoder_);
    delete instructionQueue_;     // FIXME: placement of this here is dangerous and will result in segfault! Garbage code!
    delete joystick_;
    delete ui_;
}