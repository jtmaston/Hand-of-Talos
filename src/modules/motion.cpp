//
// Created by aanas on 12/12/22.
//

#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

#ifdef __ARM_NEON
#include <arm_neon.h>
#else
typedef float float32_t;
#endif

void MainWindow::halt()
{
    emgStop_ = !emgStop_;
    if(emgStop_)
    {
        dev_.toggleTorque(false);
        dev_.toggleTorque(false);
        dev_.toggleTorque(false);
        ui_->halt_btn->setText("Reset");
        ui_->halt_btn->setProperty("alarm", true);
        ui_->halt_btn->setStyleSheet(ui_->halt_btn->styleSheet());
        //ui_->halt_btn->setStyleSheet("background-color:yellow");
        Logger::warning("Stopped by emergency signal!");
    }else
    {
        dev_.toggleTorque(true);
        ui_->halt_btn->setText("STOP");
        ui_->halt_btn->setProperty("alarm", false);
        ui_->halt_btn->setStyleSheet(ui_->halt_btn->styleSheet());
        Logger::warning("Reset from emergency");
    }


}


void MainWindow::followColor() {
    interruptFlag_ = true;
    usleep(1000);
    int x_center;
    int y_center;

    switch (dir_) {
        case 1: {
            x_center = red_.x + red_.width / 2;
            y_center = red_.y + red_.y / 2;
            break;
        }
        case 2: {
            x_center = green_.x + green_.width / 2;
            y_center = green_.y + green_.y / 2;
            break;
        }
        case 3: {
            x_center = blue_.x + blue_.width / 2;
            y_center = blue_.y + blue_.y / 2;
            break;
        }

    }
    float32_t alpha = (asin((864 / 2 - float(x_center)) / ((30.0f) * PIX_MOD)) * 180.0f) / M_PI;
    float32_t beta =  (asin((480 / 2 - float(y_center)) / ((36) * PIX_MOD)) * 180) / M_PI;

    followTarget_.at(0) += round(alpha);        // todo: errcheck before issuing!
    followTarget_.at(3) -= round(beta);

    usleep(1000);
    dev_.servoWrite6(followTarget_, 470);

}

void MainWindow::goHome() {

    Instruction move;
    move.opcode = ANGS;
    move.params.push_back(0);
    programStack_.push_back(move);

    Logger::info("Unlock from goHome");
    interruptFlag_ = true;
    interpreterLock_.unlock();
}

void MainWindow::command() // get the values from the sliders, the$n write them on the motorBus
{
    // TODO: maybe tidy up a bit?

    if ( !checkIfMovementNecessary() )
        return;

    for ( int i = 0 ; i < 6; i++) {
        dev_.angles_.at(i) = increments_.at(i)->value();
        sliders_.at(i)->setValue(static_cast<int>(increments_.at(i)->value()));
    }

    /*if (base_.position_ != ui_->increment_t->value()) {
        base_.move(ui_->increment_t->value());
    }*/

    Instruction anonymous_angs;
    anonymous_angs.opcode = ANGS;
    anonymous_angs.params.clear();
    anonymous_angs.params.push_back(8192);
    for ( auto &ang : dev_.angles_ )
        anonymous_angs.params.push_back(ang);

    programStack_.clear();
    programStack_.push_back(anonymous_angs);

    Logger::info("Unlock from command");
    interruptFlag_ = true;
    interpreterLock_.unlock();
}

inline bool MainWindow::checkIfMovementNecessary()
{
    for( int i = 0 ; i < 6; i++) {
        if (abs(dev_.angles_.at(i) - increments_.at(i)->value()) > 2)
            return true;
    }

    return false;
}
