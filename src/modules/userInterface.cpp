//
// Created by aanas on 12/12/22.
//

#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::setButtonColor(int index) {
    for(int i = 0 ; i < buttons_.size(); i++)
    {
        if(i == index) {
            buttons_.at(i)->setProperty("selectedActive", true);
            buttons_.at(i)->setStyleSheet(buttons_.at(i)->styleSheet());
        }
        else {
            buttons_.at(i)->setProperty("selectedActive", false);
            buttons_.at(i)->setStyleSheet(buttons_.at(i)->styleSheet());
        }
    }
}

void MainWindow::toggleLearnBar() // TODO: trigger to toggle the mode, not the bar
{
    jogging_ = false;
    setButtonColor(0);
    setCameraBarVisibility(HIDDEN);         // hide the camera bar
    learnBarState_ = !learnBarState_;        // << set if the learning bar is visible or not
    setLearnBarVisibility(learnBarState_); // >>
    learn();                                   // and go into the learn mode
}

void MainWindow::toggleCameraBar() // ditto for the camera bar
{
    jogging_ = false;
    setButtonColor(1);
    setLearnBarVisibility(HIDDEN);
    cameraBarState_ = !cameraBarState_;
    setCameraBarVisibility(cameraBarState_);

}

void MainWindow::setCameraBarVisibility(bool state) // hide the camera bar, by setting height to 0
{
    cameraBarState_ = state;
    ui_->follow_blue->setVisible(cameraBarState_);
    ui_->follow_red->setVisible(cameraBarState_);
    ui_->follow_green->setVisible(cameraBarState_);
    ui_->follow_stop->setVisible(cameraBarState_);
}

void MainWindow::setLearnBarVisibility(bool state) // hide the learn bar, by setting the height to 0
{
    learnBarState_ = state;
    ui_->learnModeBar->setVisible(learnBarState_);

}

void MainWindow::startFollowRed()
{
    switch (dir_)
    {
        case 0:
        {
            dir_ = 1;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(followColor()));
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    dir_ = 1;

    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 0};

    /*write(dev_.ledBus_, mode, 2);     // TODO: set rgb
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);*/
}

void MainWindow::startFollowGreen()
{
    switch (dir_)
    {
        case 0:
        {
            dir_ = 2;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(followColor()));
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    dir_ = 2;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 1};

    /*write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);*/
}

void MainWindow::startFollowBlue()
{
    switch (dir_)
    {
        case 0:
        {
            dir_ = 3;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(followColor()));
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    dir_ = 3;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 2};

    /*write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);*/
}

void MainWindow::stopFollow()
{
    //disconnect(scheduler500Ms_, SIGNAL(timeout()), this, SLOT(followColor()));
    //connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    //write(dev_.ledBus_, cmd, 2);
}

// Source: https://stackoverflow.com/a/16606128
#include <sstream>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 1)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

// End source
void MainWindow::updateAxes() // this updates the axes display
{
    std::array<float, 6> data = dev_.servoReadall(); // read the values from all of the servos

    for(int i = 0 ; i < 6; i++)
    {
        dev_.angles_.at(i) = data.at(i);
    }

    ui_->a1_d->setText(std::string(std::string("Axis 1: ") + to_string_with_precision(dev_.angles_[0])  +
                                   "°").c_str()); // and set the strings for
    ui_->a2_d->setText(std::string(
            std::string("Axis 2: ") + to_string_with_precision(dev_.angles_[1]) + "°").c_str()); // the labels
    ui_->a3_d->setText(
            std::string(std::string("Axis 3: ") + to_string_with_precision(dev_.angles_[2]) + "°").c_str());
    ui_->a4_d->setText(
            std::string(std::string("Axis 4: ") + to_string_with_precision(dev_.angles_[3]) + "°").c_str());
    ui_->a5_d->setText(
            std::string(std::string("Axis 5: ") + to_string_with_precision(dev_.angles_[4]) + "°").c_str());
    ui_->a6_d->setText(std::string("Gripper: " + to_string_with_precision(int(dev_.angles_[5])) + "%").c_str());

    std::array<float, 16> end_effector{};
    dev_.calculateEndEffector(end_effector);

    ui_->base_x->setText("X: " + QString(to_string_with_precision(end_effector[12]).c_str()) + "mm");
    ui_->base_y->setText("Y: " + QString(to_string_with_precision(end_effector[13]).c_str()) + "mm");
    ui_->base_z->setText("Z: " + QString(to_string_with_precision(end_effector[14]).c_str()) + "mm");

    ui_->base_Rx->setText(
            "Rx: " + QString(to_string_with_precision(atan(end_effector[5] / end_effector[8]) * DEG_RAD).c_str()) +
            "°");
    ui_->base_Ry->setText(
            "Ry: " + QString(to_string_with_precision(asin(-end_effector[2]) * DEG_RAD).c_str()) + "°");
    ui_->base_Rz->setText(
            "Rz: " + QString(to_string_with_precision(atan(end_effector[1] / end_effector[0]) * DEG_RAD).c_str()) +
            "°");

}