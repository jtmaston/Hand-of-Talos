#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::learn() // starts the learn mode
{
    // disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command())); // stop the control function
    // go_home();                                                             // move back to home
    dev.toggleTorque(false); // and disable the torque
    learning = true;
}

void MainWindow::add_step() // add a step
{
    float32_t *angle = dev.servo_readall(); // read all the servo values
    //std::vector<float32_t> t(6); // make them into a vector


    Instruction local;
    local.opcode = TGT;
    local.params[0] = manual_program.size();
    memcpy(local.params + 1, angle, 5 * sizeof(float32_t));
    manual_program.push_back(local);

    local.opcode = ANGS;
    local.params[0] = local.params[0];
    manual_program.push_back(local);
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str())); // then update the label
    delete angle;
}

void MainWindow::remove_step() // remove a step from the queue
{
    dev.learned_angles.pop_back();
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str())); // then update the label
}

void MainWindow::follow_path() // start running
{
    //prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, manual_program); FIXME:
}