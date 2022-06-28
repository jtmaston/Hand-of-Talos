#include "mainwindow.hpp"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);

    init_signals();
    init_peripherals();

    running = true;
    prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, instruction_queue, interrupt_vector, running, active);
    dev.toggleTorque(true);
}

MainWindow::~MainWindow()
{
    running = false;
    cam_thread.waitForFinished();
    joy_thread.waitForFinished();
    prog_thread.waitForFinished();

    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
    quirc_destroy(decoder);
    delete joystick;
    delete ui;
}