#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::toggleLearnBar() // toggle the learning bar,
{
    setCameraBarVisibility(HIDDEN);         // hide the camera bar
    learnBarVisibility_ = !learnBarVisibility_;        // << set if the learning bar is visible or not
    setLearnBarVisibility(learnBarVisibility_); // >>
    startLearnMode();                                   // and go into the learn mode
}

void MainWindow::toggleCameraBar() // ditto for the camera bar
{
    setLearnBarVisibility(HIDDEN);
    cameraBarVisibility = !cameraBarVisibility;
    setCameraBarVisibility(cameraBarVisibility);
}

void MainWindow::setCameraBarVisibility(bool state) // hide the camera bar, by setting height to 0
{
    cameraBarVisibility = state;
    ui->follow_blue->setVisible(cameraBarVisibility);
    ui->follow_red->setVisible(cameraBarVisibility);
    ui->follow_green->setVisible(cameraBarVisibility);
    ui->follow_stop->setVisible(cameraBarVisibility);
}

void MainWindow::setLearnBarVisibility(bool state) // hide the learn bar, by setting the height to 0
{
    learnBarVisibility_ = state;
    ui->remove_step->setVisible(learnBarVisibility_);
    ui->run_learned->setVisible(learnBarVisibility_);
    ui->add_step->setVisible(learnBarVisibility_);
    ui->factor_label->setVisible(learnBarVisibility_);
    ui->factor_slider->setVisible(learnBarVisibility_);
    ui->factor_box->setVisible(learnBarVisibility_);
    ui->save_button->setVisible(learnBarVisibility_);
}



void MainWindow::updateAxisDisplay() // this updates the axes display
{
    

    ui->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(dev_.angles[0]).substr(0, 5) + "°").c_str()); // and set the strings for
    ui->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(dev_.angles[1]).substr(0, 5) + "°").c_str()); // the labels
    ui->a3_d->setText(std::string(std::string("Axis 3: ") + std::to_string(dev_.angles[2]).substr(0, 5) + "°").c_str());
    ui->a4_d->setText(std::string(std::string("Axis 4: ") + std::to_string(dev_.angles[3]).substr(0, 5) + "°").c_str());
    ui->a5_d->setText(std::string(std::string("Axis 5: ") + std::to_string(dev_.angles[4]).substr(0, 5) + "°").c_str());
    ui->a6_d->setText(std::string("Gripper: " + std::to_string(int(dev_.angles[5])) + "%").c_str());

    float32_t end_effector[16];
    dev_.calculate_end_effector(end_effector);

    ui->base_x->setText("X: " + QString(std::to_string(end_effector[12]).substr(0, 5).c_str()) + "mm");
    ui->base_y->setText("Y: " + QString(std::to_string(end_effector[13]).substr(0, 5).c_str()) + "mm");
    ui->base_z->setText("Z: " + QString(std::to_string(end_effector[14]).substr(0, 5).c_str()) + "mm");

    ui->base_Rx->setText("Rx: " + QString(std::to_string(atan(end_effector[5] / end_effector[8]) * degRad).substr(0, 5).c_str()) + "°");
    ui->base_Ry->setText("Ry: " + QString(std::to_string(asin(-end_effector[2]) * degRad).substr(0, 5).c_str()) + "°");
    ui->base_Rz->setText("Rz: " + QString(std::to_string(atan(end_effector[1] / end_effector[0]) * degRad).substr(0, 5).c_str()) + "°");

    
}