//
// Created by aanas on 12/12/22.
//
#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

#include "RobotArm.hpp"
#include <QProgressDialog>
void MainWindow::learn() // starts the learn mode
{
    disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command())); // stop the control function
    goHome();// move back to home

    QThread::msleep(1200); // TODO: add popup
    dev_.toggleTorque(false); // and disable the torque
    programStack_.clear();
}

void MainWindow::addStep() // add a step
{
    //float32_t *angle = dev_.servoReadall(); // read all the servo values
    std::vector<float32_t> t(6); // make them into a vector

    Instruction local;
    local.opcode = TGT;
    local.params.clear();
    local.params.push_back(programStack_.size() / 2 + 1);

    for ( int i = 0 ; i < 6; i ++ )
    {
        /*switch(i){
            case 3:
                local.params.push_back(dev_.angles_.at(i) + 180);
                break;
            default:
                local.params.push_back(dev_.angles_.at(i) + 90);
                break;
        }*/
        local.params.push_back(dev_.angles_.at(i));
    }

    programStack_.push_back(local);


    local.params.clear();
    local.opcode = ANGS;
    local.params.push_back(programStack_.at(programStack_.size() - 1).params.at(0));
    programStack_.push_back(local);
    ui_->execute->setText(QString(std::to_string(programStack_.size() / 2 ).c_str())); // then update the label
    // delete angle;
}

void MainWindow::removeStep() // remove a step from the queue
{
    if(!programStack_.empty()) {
        programStack_.pop_back();
        programStack_.pop_back();
    }
    ui_->execute->setText(QString(std::to_string(programStack_.size() / 2).c_str())); // then update the label
}

void MainWindow::followLearnedPath() // start running
{
    dev_.toggleTorque(true);

    std::cout << programStack_.size() << '\n';
    interpreterLock_.unlock();
}