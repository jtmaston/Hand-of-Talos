#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::command() // get the values from the sliders, then write them on the bus
{
    float32_t angles[6];
    angles[0] = ui->increment_1->value() + dev.home_position[0]; // need to adjust with 90
    angles[1] = ui->increment_2->value() + dev.home_position[1];
    angles[2] = ui->increment_3->value() + dev.home_position[2];
    angles[3] = ui->increment_4->value() + dev.home_position[3];
    angles[4] = ui->increment_5->value() + dev.home_position[4];
    angles[5] = ui->increment_6->value() + dev.home_position[5];

    ui->base_r->setValue(static_cast<int>(ui->increment_1->value()));
    ui->a2_r->setValue(static_cast<int>(ui->increment_2->value()));
    ui->a3_r->setValue(static_cast<int>(ui->increment_3->value()));
    ui->a4_r->setValue(static_cast<int>(ui->increment_4->value()));
    ui->a5_r->setValue(static_cast<int>(ui->increment_5->value()));
    ui->grip_r->setValue(static_cast<int>(ui->increment_6->value()));

    if (base.position != ui->increment_t->value())
    {
        base.move(ui->increment_t->value());
    }
    
    Instruction target;
    target.opcode = TGT;
    Instruction move;
    move.opcode = ANGS;
    move.params[0] = 8192;
    memmove(move.params + 1, angles, 6 * sizeof(float));
    instruction_queue.push_back(std::move(move));
}

void MainWindow::halt()
{
    dev.toggleTorque(false);
}

void MainWindow::go_home()
{
    ui->base_r->setValue(dev.home_position[0] - 90);
    ui->a2_r->setValue(dev.home_position[1] - 90);
    ui->a3_r->setValue(dev.home_position[2] - 90);
    ui->a4_r->setValue(dev.home_position[3] - 90);
    ui->a5_r->setValue(dev.home_position[4] - 90);
    ui->grip_r->setValue(dev.home_position[5]);
}