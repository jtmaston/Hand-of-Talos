//
// Created by aanas on 12/12/22.
//

#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::toggleLearnBar() // toggle the learning bar,
{
    setCameraBarVisibility(HIDDEN);         // hide the camera bar
    learnBarState_ = !learnBarState_;        // << set if the learning bar is visible or not
    setLearnBarVisibility(learnBarState_); // >>
    learn();                                   // and go into the learn mode
}

void MainWindow::toggleCameraBar() // ditto for the camera bar
{
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
    ui_->execute->setVisible(learnBarState_);
    ui_->next->setVisible(learnBarState_);
    ui_->prev->setVisible(learnBarState_);
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

    write(dev_.led_bus, mode, 2);
    write(dev_.led_bus, speed, 2);
    write(dev_.led_bus, color, 2);
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

    write(dev_.led_bus, mode, 2);
    write(dev_.led_bus, speed, 2);
    write(dev_.led_bus, color, 2);
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

    write(dev_.led_bus, mode, 2);
    write(dev_.led_bus, speed, 2);
    write(dev_.led_bus, color, 2);
}

void MainWindow::stopFollow()
{
    //disconnect(scheduler500Ms_, SIGNAL(timeout()), this, SLOT(followColor()));
    //connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    write(dev_.led_bus, cmd, 2);
}

void MainWindow::updateAxes() // this updates the axes display
{
    float32_t *data = dev_.servo_readall(); // read the values from all of the servos
    // dev.angles_ = data;

    dev_.angles_.clear();           // TODO: this is bad :(


    dev_.angles_.push_back(data[0] - dev_.homePosition_[0]);
    dev_.angles_.push_back(-(data[1] - dev_.homePosition_[1]));
    dev_.angles_.push_back(-(data[2] - dev_.homePosition_[2]));
    dev_.angles_.push_back(-(data[3] - dev_.homePosition_[3] + 170));
    dev_.angles_.push_back(data[4] - dev_.homePosition_[4]);
    dev_.angles_.push_back(data[5] - dev_.homePosition_[5]);

    // memcpy(dev.angles_.data(), data, 6 * sizeof(float32_t));
    ui_->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(dev_.angles_[0]).substr(0, 5) +
                                   "°").c_str()); // and set the strings for
    ui_->a2_d->setText(std::string(
            std::string("Axis 2: ") + std::to_string(dev_.angles_[1]).substr(0, 5) + "°").c_str()); // the labels
    ui_->a3_d->setText(
            std::string(std::string("Axis 3: ") + std::to_string(dev_.angles_[2]).substr(0, 5) + "°").c_str());
    ui_->a4_d->setText(
            std::string(std::string("Axis 4: ") + std::to_string(dev_.angles_[3]).substr(0, 5) + "°").c_str());
    ui_->a5_d->setText(
            std::string(std::string("Axis 5: ") + std::to_string(dev_.angles_[4]).substr(0, 5) + "°").c_str());
    ui_->a6_d->setText(std::string("Gripper: " + std::to_string(int(dev_.angles_[5])) + "%").c_str());

    float32_t end_effector[16];
    dev_.calculateEndEffector(end_effector);

    ui_->base_x->setText("X: " + QString(std::to_string(end_effector[12]).substr(0, 5).c_str()) + "mm");
    ui_->base_y->setText("Y: " + QString(std::to_string(end_effector[13]).substr(0, 5).c_str()) + "mm");
    ui_->base_z->setText("Z: " + QString(std::to_string(end_effector[14]).substr(0, 5).c_str()) + "mm");

    ui_->base_Rx->setText(
            "Rx: " + QString(std::to_string(atan(end_effector[5] / end_effector[8]) * DEG_RAD).substr(0, 5).c_str()) +
            "°");
    ui_->base_Ry->setText(
            "Ry: " + QString(std::to_string(asin(-end_effector[2]) * DEG_RAD).substr(0, 5).c_str()) + "°");
    ui_->base_Rz->setText(
            "Rz: " + QString(std::to_string(atan(end_effector[1] / end_effector[0]) * DEG_RAD).substr(0, 5).c_str()) +
            "°");

    delete data; // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}