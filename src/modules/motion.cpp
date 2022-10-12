#include "mainwindow.hpp"
#include "ui_mainwindow.h"


void MainWindow::moveToPosition() // get the values from the sliders, then write them on the bus_
{
    //std::cout << "Here!";
    float32_t angles[6];
    angles[0] = (float) ui_->increment_1->value() + dev_.homePosition_[0]; // need to adjust with 90
    angles[1] = (float) ui_->increment_2->value() + dev_.homePosition_[1];
    angles[2] = (float) ui_->increment_3->value() + dev_.homePosition_[2];
    angles[3] = (float) ui_->increment_4->value() + dev_.homePosition_[3];
    angles[4] = (float) ui_->increment_5->value() + dev_.homePosition_[4];
    angles[5] = (float) ui_->increment_6->value() + dev_.homePosition_[5];

    ui_->base_r->setValue(static_cast<int>(ui_->increment_1->value()));
    ui_->a2_r->setValue(static_cast<int>(ui_->increment_2->value()));
    ui_->a3_r->setValue(static_cast<int>(ui_->increment_3->value()));
    ui_->a4_r->setValue(static_cast<int>(ui_->increment_4->value()));
    ui_->a5_r->setValue(static_cast<int>(ui_->increment_5->value()));
    ui_->grip_r->setValue(static_cast<int>(ui_->increment_6->value()));

    if (translationAxis_.position_ != ui_->increment_t->value() && translationAxis_.active_) {
        translationAxis_.move(ui_->increment_t->value());
    }
    Instruction movement;
    movement.params.clear();

    movement.opcode = ANGS;
    movement.params.push_back(8192);
    for (float &angle: angles)
        movement.params.push_back(angle);

    instructionQueue_->push_back(std::move(movement));
    antiFreewheel_.unlock();
}

void MainWindow::emergencyStop() {
    dev_.toggleTorque(false);
}

void MainWindow::goToHomePosition() {
    Instruction movement;
    movement.opcode = ANGS;
    movement.params[0] = 8192;
    memmove(movement.params.data() + 1, dev_.homePosition_.data(), 6 * sizeof(float));
    instructionQueue_->push_back(std::move(movement));
    antiFreewheel_.unlock();
}