#include "mainwindow.hpp"
#include "ui_mainwindow.h"


void MainWindow::startLearnMode() // starts the learn mode
{

    QMessageBox* movement_warning = new QMessageBox(this);
    movement_warning->setWindowTitle("Information");
    movement_warning->setText("Please wait for return to origin!");
    movement_warning->setStandardButtons(0);

    switch (learnModeActive_)
    {
    case 0:
    {
        movement_warning->open();
        disconnect(Scheduler_100ms_, SIGNAL(timeout()), this, SLOT(moveToPosition())); // stop the control function
        dev_.timeFactor = 1000;
        goToHomePosition(); // move back to home

        crappyDelay(2500);

        dev_.toggleTorque(false); // and disable the torque
        runningLearnMode_ = true;
        manualProgramStack_.clear();

        antiFreewheel_.unlock(); // allow the interpreter to flush itself, then lock.
        ui->axes_box->setVisible(false);
        movement_warning->close();
    
    break;
    }
    case 1:

        movement_warning->open();
        dev_.toggleTorque(true);
        goToHomePosition();
        crappyDelay(2500);                                            // move back to home
        connect(Scheduler_100ms_, SIGNAL(timeout()), this, SLOT(moveToPosition())); // stop the control function
        movement_warning->close();     
        ui->axes_box->setVisible(true);                                       // and disable the torque
        break;
    }
    delete movement_warning;
    learnModeActive_ = !learnModeActive_;
}

void MainWindow::addStep() // add a step
{
    Instruction local;
    local.opcode = TGT;
    local.params[0] = manualProgramStack_.size() / 2;
    memcpy(local.params.data() + 1, dev_.angles.data(), 5 * sizeof(float32_t));

    for (int i = 0; i < 5; i++)
        local.params[i + 1] += dev_.home_position[i];

    manualProgramStack_.push_back(local);

    local.opcode = ANGS;
    local.params[0] = local.params[0];
    manualProgramStack_.push_back(local);
    // ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str())); // then update the label
    // delete angle;
}

void MainWindow::removeStep() // remove a step from the queue
{
    dev_.learned_angles.pop_back();
    // ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str())); // then update the label
}

void MainWindow::startFollowingPath() // start running
{
    dev_.timeFactor = 1000;

    dev_.toggleTorque(true);
    goToHomePosition();
    usleep(1500e3);

    *instructionQueue_ = manualProgramStack_;
    waitForMoveComplete_ = false;
    antiFreewheel_.unlock(); // ready to go, unlock
}