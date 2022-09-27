#include "mainwindow.hpp"
#include "ui_mainwindow.h"



void MainWindow::moveToPosition() // get the values from the sliders, then write them on the bus
{
    float32_t angles[6];
    angles[0] = ui->increment_1->value() + dev_.homePosition_[0]; // need to adjust with 90
    angles[1] = ui->increment_2->value() + dev_.homePosition_[1];
    angles[2] = ui->increment_3->value() + dev_.homePosition_[2];
    angles[3] = ui->increment_4->value() + dev_.homePosition_[3];
    angles[4] = ui->increment_5->value() + dev_.homePosition_[4];
    angles[5] = ui->increment_6->value() + dev_.homePosition_[5];

    ui->base_r->setValue(static_cast<int>(ui->increment_1->value()));
    ui->a2_r->setValue(static_cast<int>(ui->increment_2->value()));
    ui->a3_r->setValue(static_cast<int>(ui->increment_3->value()));
    ui->a4_r->setValue(static_cast<int>(ui->increment_4->value()));
    ui->a5_r->setValue(static_cast<int>(ui->increment_5->value()));
    ui->grip_r->setValue(static_cast<int>(ui->increment_6->value()));

    if (translationAxis_.position != ui->increment_t->value() && translationAxis_.active)
    {   
        translationAxis_.move(ui->increment_t->value());
    }
    Instruction movement;
    movement.params.clear();

    movement.opcode = ANGS;
    movement.params.push_back (8192);
    for(int i = 0 ; i < 6; i++)
        movement.params.push_back(angles[i]);

    instructionQueue_ -> push_back(std::move(movement));
    antiFreewheel_.unlock();
}

void MainWindow::emergencyStop()
{
    dev_.toggleTorque(false);
}

void MainWindow::goToHomePosition()
{
    Instruction movement;
    movement.opcode = ANGS;
    movement.params[0] = 8192;
    memmove(movement.params.data()  + 1, dev_.homePosition_.data(), 6 * sizeof(float));
    instructionQueue_ -> push_back(std::move(movement));
    antiFreewheel_.unlock();
}