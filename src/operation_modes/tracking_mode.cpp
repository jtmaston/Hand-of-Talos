#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::setTrackingColorRed() {
    ui_->axes_box->setVisible(false);
    switch (trackingDirection_) {
        case 0: {
            trackingDirection_ = 1;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(startColorTracking()));
            //disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    trackingDirection_ = 1;

    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 0};

    write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);
}

void MainWindow::setTrackingColorGreen() {
    ui_->axes_box->setVisible(false);
    switch (trackingDirection_) {
        case 0: {
            trackingDirection_ = 2;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(startColorTracking()));
            //disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    trackingDirection_ = 2;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 1};

    write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);
}

void MainWindow::setTrackingColorBlue() {
    ui_->axes_box->setVisible(false);
    switch (trackingDirection_) {
        case 0: {
            trackingDirection_ = 3;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(startColorTracking()));
            //disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    trackingDirection_ = 3;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 2};

    write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);
}

void MainWindow::stopTracking() {
    ui_->axes_box->setVisible(true);

    disconnect(scheduler500Ms_, SIGNAL(timeout()), this, SLOT(startColorTracking()));
    //connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    write(dev_.ledBus_, cmd, 2);
}

void MainWindow::startColorTracking() {
    int x_center = 640 / 2;
    int y_center = 480 / 2;

    switch (trackingDirection_) {
        case 1: {
            x_center = redBoundingBox_.x + redBoundingBox_.width / 2;
            y_center = redBoundingBox_.y + redBoundingBox_.y / 2;
            break;
        }
        case 2: {
            x_center = greenBoundingBox_.x + greenBoundingBox_.width / 2;
            y_center = greenBoundingBox_.y + greenBoundingBox_.y / 2;
            break;
        }
        case 3: {
            x_center = blueBoundingBox_.x + blueBoundingBox_.width / 2;
            y_center = blueBoundingBox_.y + blueBoundingBox_.y / 2;
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

    //dev.servoWrite6(angles, 450);        FIXME:
}