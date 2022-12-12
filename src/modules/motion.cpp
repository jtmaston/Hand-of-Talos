//
// Created by aanas on 12/12/22.
//

#include "inc/mainwindow.h"
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

    dev_.servo_write6(angles, 450);
}

void MainWindow::followLearnedPath() // start running
{
    //prog_thread = QtConcurrent::run(this, &MainWindow::rasmInterpreter, dev.homePosition_, manual_program);
    std::cout << "Here\n";

    /*dev.button_mode(0);
    uint8_t buf[4] = { 0x20, 1 };
    write( dev.bus, buf, 2);*/
    uint8_t buf[4] = {0x03, 0};
    write(dev_.bus, buf, 2);
    usleep(100);
    buf[0] = 0x22, buf[1] = 0x01;
    write(dev_.bus, buf, 2);
    usleep(100);
    uint8_t a = i2c_smbus_read_byte_data(dev_.bus, 0x22);
    std::cout << (int) a << '\n';

    buf[0] = 0x1A, buf[1] = 0x01;
    write(dev_.bus, buf, 2);

    usleep(100);

    buf[0] = 0x20, buf[1] = 0x01;
    write(dev_.bus, buf, 2);

    usleep(100);
    buf[0] = 0x23, buf[1] = 0x01;
    write(dev_.bus, buf, 2);

}

void MainWindow::goHome() {
    ui_->base_r->setValue(dev_.homePosition_[0] - 90);
    ui_->a2_r->setValue(dev_.homePosition_[1] - 90);
    ui_->a3_r->setValue(dev_.homePosition_[2] - 90);
    ui_->a4_r->setValue(dev_.homePosition_[3] - 90);
    ui_->a5_r->setValue(dev_.homePosition_[4] - 90);
    ui_->grip_r->setValue(dev_.homePosition_[5]);
}

void MainWindow::command() // get the values from the sliders, then write them on the bus
{
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

    if (base_.position_ != ui_->increment_t->value()) {
        base_.move(ui_->increment_t->value());
    }

    dev_.servo_write6(angles, timeMod_); // move the axes
}