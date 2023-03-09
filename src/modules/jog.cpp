//
// Created by aanas on 12/12/22.
//
#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"


void MainWindow::jog() {

    /*if (!jogging_) {
        connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(updateStick()));
        timeMod_ = 300;
        dev_.toggleTorque(true);
        //if (learning_)
           // connect(scheduler100Ms_, SIGNAL(timeout()),
                //    SLOT(command())); // control from the axis is also updated ever 100ms

    } else {
        disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(updateStick()));
        timeMod_ = 1000;
    }
    jogging_ = !jogging_;
    toggleJog();*/
    //std::cout << following_program << '\n';
    //std::cout.flush();
}

/*void MainWindow::updateStick() {
    ui_->base_r->setValue(ui_->base_r->value() - joystick_->axisLeftX() * 100 * 0.05);
    ui_->a2_r->setValue(ui_->a2_r->value() - joystick_->axisLeftY() * 100 * 0.05);
    ui_->a3_r->setValue(ui_->a3_r->value() - joystick_->axisRightX() * 100 * 0.05);
    ui_->a4_r->setValue(ui_->a4_r->value() - joystick_->axisRightY() * 100 * 0.05);
    ui_->a5_r->setValue(ui_->a5_r->value() - joystick_->buttonLeft() * 0.05);
    ui_->a5_r->setValue(ui_->a5_r->value() + joystick_->buttonRight() * 0.05);

    ui_->grip_r->setValue(ui_->grip_r->value() - round(axes_[5] * 100) * 0.1);
}*/

void MainWindow::toggleJog() {
    //setButtonColor(2);

    setCameraBarVisibility(false);
    setLearnBarVisibility(false);

    jogging_ = !jogging_;
    ui_->jogSliders->setVisible(jogging_);
}