#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void arbitrator()
{
    
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);

    init_signals();
    init_peripherals();
    init_device();


    instruction_queue = new std::vector<Instruction>;

    running = true;
    nodelay = true;
    this->time_mod = 5000;
    prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, instruction_queue, interrupt_vector, &running, &active);
    dev.toggleTorque(true);
}

MainWindow::~MainWindow()
{
    running = false;
    anti_freewheel.unlock();
    cam_thread.waitForFinished();
    prog_thread.waitForFinished();

    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
    quirc_destroy(decoder);
    delete instruction_queue;     // FIXME: placement of this here is dangerous and will result in segfault!
    delete joystick;
    delete ui;
}