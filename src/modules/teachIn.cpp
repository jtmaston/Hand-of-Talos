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
Instruction previous_target;

#include <exception>
void MainWindow::addStep() // add a step
{

    Instruction local;
    local.opcode = TGT;
    local.params.clear();
    local.params.push_back(programStack_.size() / 2 + 1);

    for ( int i = 0 ; i < 6; i ++ )
    {
        if( learningTrajectory_ && programStack_.size() > 3) {
            if (abs(previous_target.params.at(i + 1) - dev_.angles_.at(i)) > 10){
                Logger::warning("Rejected angle! Difference was " +
                                std::to_string(abs(previous_target.params.at(i + 1) - dev_.angles_.at(i))));
                local.params.push_back(previous_target.params.at(i+1));
            }else
                local.params.push_back(dev_.angles_.at(i));
        }else
            local.params.push_back(dev_.angles_.at(i));
    }

    programStack_.push_back(std::move(local));
    previous_target = programStack_.at(programStack_.size() - 1);


    Instruction angs;
    angs.params.clear();
    angs.opcode = ANGS;
    angs.params.push_back(programStack_.at(programStack_.size() - 1).params.at(0));
    programStack_.push_back(std::move(angs));
    //ui_->execute->setText(QString(std::to_string(programStack_.size() / 2 ).c_str())); // then update the label
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

void MainWindow::learnTrajectory()
{
    if(!learningTrajectory_)
    {
        Instruction speedMod;
        speedMod.opcode = SPD;
        speedMod.params.push_back(100);
        programStack_.push_back(speedMod);
        connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(addStep()));
        ui_->TrajectoryToggleButton->setText("Trajectory learning: ON");
    }else
    {
        disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(addStep()));
        Instruction speedMod;
        speedMod.opcode = SPD;
        speedMod.params.push_back(1000);    // todo: check if command sets timeMod
        programStack_.push_back(speedMod);
        programStack_.push_back(speedMod);
        ui_->TrajectoryToggleButton->setText("Trajectory learning: OFF");
    }
    learningTrajectory_ = !learningTrajectory_;

}

void MainWindow::saveProgramToDisk(){
    QString path = QFileDialog::getSaveFileName(this,
                                             "Save as: ", "./",
                                             "Compiled robot Assembly Files (*.bin)");
    std::ofstream fout(path.toStdString());
    for( const auto&instr : programStack_)
    {
        fout << instr.opcode << ' ';
        for(const auto& param : instr.params)
            fout << param << ' ';
        fout << '\n';
    }
}
void MainWindow::changeToLoop(){
    programInLoop_ = !programInLoop_;
    if(programInLoop_)
        ui_->RunModeToggleButton->setText("Running Mode: Loop");
    else
        ui_->RunModeToggleButton->setText("Running Mode: Single-shot");
}