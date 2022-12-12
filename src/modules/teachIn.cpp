//
// Created by aanas on 12/12/22.
//
#include "inc/mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::learn() // starts the learn mode
{
    disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command())); // stop the control function
    // goHome();                                                             // move back to home
    //dev.toggleTorque(false); // and disable the torque

    uint_fast8_t buf[20] = {0x03, 0x01};
    write(dev_.bus, buf, 2);
    buf[0] = 0x24;
    buf[1] = 0x01;
    write(dev_.bus, buf, 2);
}

void MainWindow::addStep() // add a step
{
    // float32_t *angle = dev.servo_readall(); // read all the servo values
    std::vector<float32_t> t(6); // make them into a vector

    t[0] = dev_.angles_[0];
    t[1] = dev_.angles_[1];
    t[2] = dev_.angles_[2];
    t[3] = (dev_.angles_[3]);
    t[4] = (dev_.angles_[4]);
    t[5] = (dev_.angles_[5]);

    Instruction local;
    local.opcode = TGT;
    local.params[0] = manualProgram_.size();
    memcpy(local.params.data() + 1, t.data(), 5 * sizeof(float32_t));
    manualProgram_.push_back(local);

    local.opcode = ANGS;
    local.params[0] = local.params[0];
    manualProgram_.push_back(local);
    ui_->execute->setText(QString(std::to_string(ui_->execute->text().toInt() + 1).c_str())); // then update the label
    // delete angle;
}

void MainWindow::removeStep() // remove a step from the queue
{
    dev_.learned_angles.pop_back();
    disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
    ui_->execute->setText(QString(std::to_string(ui_->execute->text().toInt() - 1).c_str())); // then update the label
}