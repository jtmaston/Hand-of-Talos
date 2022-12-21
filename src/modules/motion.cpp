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

void MainWindow::followColor() {
    int x_center = 640 / 2;
    int y_center = 480 / 2;

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
    float32_t alpha = (asin((320 - x_center) / ((22) * PIX_MOD)) * 180) / M_PI;
    float32_t beta = (asin((240 - y_center) / ((36) * PIX_MOD)) * 180) / M_PI;

    ui_->base_r->setValue(ui_->base_r->value() + round(alpha));
    ui_->a4_r->setValue(ui_->a4_r->value() - round(beta));

    float32_t angles[6];
    angles[0] = ui_->increment_1->value() + dev_.homePosition_[0]; // need to adjust with 90
    angles[1] = ui_->increment_2->value() + dev_.homePosition_[1];
    angles[2] = ui_->increment_3->value() + dev_.homePosition_[2];
    angles[3] = ui_->increment_4->value() + dev_.homePosition_[3];
    angles[4] = ui_->increment_5->value() + dev_.homePosition_[4];
    angles[5] = ui_->increment_6->value() + dev_.homePosition_[5];

    ui_->base_r->setValue(static_cast<int>(ui_->increment_1->value()));
    ui_->a2_r->setValue(static_cast<int>(ui_->increment_2->value()));
    ui_->a3_r->setValue(static_cast<int>(ui_->increment_3->value()));
    ui_->a4_r->setValue(static_cast<int>(ui_->increment_4->value()));
    ui_->a5_r->setValue(static_cast<int>(ui_->increment_5->value()));
    ui_->grip_r->setValue(static_cast<int>(ui_->increment_6->value()));

    /*Instruction anonymous_angs;
    anonymous_angs.opcode = ANGS;
    for(int)*/
    //dev_.servo_write6(angles, 450);
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

void MainWindow::command() // get the values from the sliders, the$n write them on the bus
{
    // TODO: maybe tidy up a bit?

    if ( !checkIfMovementNecessary() )
        return;

    for ( int i = 0 ; i < 6; i++) {
        dev_.angles_.at(i) = increments_.at(i)->value() + dev_.homePosition_.at(i);
        sliders_.at(i)->setValue(static_cast<int>(increments_.at(i)->value()));
    }

    if (base_.position_ != ui_->increment_t->value()) {
        base_.move(ui_->increment_t->value());
    }

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
        if (abs(dev_.angles_.at(i) - increments_.at(i)->value()) > 1)
            return true;
    }

    return false;
}
