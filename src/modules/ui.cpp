#include <cmath>

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::toggleLearnBar() // toggle the learning bar,
{
    setCameraBarVisibility(HIDDEN);         // hide the camera bar
    learnBarVisibility_ = !learnBarVisibility_;        // << set if the learning bar is visible or not
    setLearnBarVisibility(learnBarVisibility_); // >>
    startLearnMode();                                   // and go into the learn mode
}

void MainWindow::toggleCameraBar() // ditto for the camera bar
{
    setLearnBarVisibility(HIDDEN);
    cameraBarVisibility_ = !cameraBarVisibility_;
    setCameraBarVisibility(cameraBarVisibility_);
}

void MainWindow::setCameraBarVisibility(bool state) // hide the camera bar, by setting height_ to 0
{
    cameraBarVisibility_ = state;
    ui_->follow_blue->setVisible(cameraBarVisibility_);
    ui_->follow_red->setVisible(cameraBarVisibility_);
    ui_->follow_green->setVisible(cameraBarVisibility_);
    ui_->follow_stop->setVisible(cameraBarVisibility_);
}

void MainWindow::setLearnBarVisibility(bool state) // hide the learn bar, by setting the height_ to 0
{
    learnBarVisibility_ = state;
    ui_->remove_step->setVisible(learnBarVisibility_);
    ui_->run_learned->setVisible(learnBarVisibility_);
    ui_->add_step->setVisible(learnBarVisibility_);
    ui_->factor_label->setVisible(learnBarVisibility_);
    ui_->factor_slider->setVisible(learnBarVisibility_);
    ui_->factor_box->setVisible(learnBarVisibility_);
    ui_->save_button->setVisible(learnBarVisibility_);
}


void MainWindow::updateAxisDisplay() // this updates the axes display
{


    ui_->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(dev_.currentPosition_[0]).substr(0, 5) +
                                   "°").c_str()); // and set the strings for
    ui_->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(dev_.currentPosition_[1]).substr(0, 5) +
                                   "°").c_str()); // the labels
    ui_->a3_d->setText(
            std::string(std::string("Axis 3: ") + std::to_string(dev_.currentPosition_[2]).substr(0, 5) + "°").c_str());
    ui_->a4_d->setText(
            std::string(std::string("Axis 4: ") + std::to_string(dev_.currentPosition_[3]).substr(0, 5) + "°").c_str());
    ui_->a5_d->setText(
            std::string(std::string("Axis 5: ") + std::to_string(dev_.currentPosition_[4]).substr(0, 5) + "°").c_str());
    ui_->a6_d->setText(std::string("Gripper: " + std::to_string(int(dev_.currentPosition_[5])) + "%").c_str());

    std::array<float32_t, 17> end_effector;
    dev_.calculateEndEffector(end_effector);

    ui_->base_x->setText("X: " + QString(std::to_string(end_effector[12]).substr(0, 5).c_str()) + "mm");
    ui_->base_y->setText("Y: " + QString(std::to_string(end_effector[13]).substr(0, 5).c_str()) + "mm");
    ui_->base_z->setText("Z: " + QString(std::to_string(end_effector[14]).substr(0, 5).c_str()) + "mm");

    ui_->base_Rx->setText("Rx: " +
                          QString(std::to_string(std::atan(end_effector[5] / end_effector[8]) * DEG_RAD).substr(0,
                                                                                                               5).c_str()) +
                          "°");
    ui_->base_Ry->setText("Ry: " + QString(std::to_string(asin(-end_effector[2]) * DEG_RAD).substr(0, 5).c_str()) + "°");
    ui_->base_Rz->setText("Rz: " +
                          QString(std::to_string(std::atan(end_effector[1] / end_effector[0]) * DEG_RAD).substr(0,
                                                                                                               5).c_str()) +
                          "°");


}