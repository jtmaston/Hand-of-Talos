#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::toggle_learn_bar() // toggle the learning bar,
{
    set_camera_bar_visibility(HIDDEN);         // hide the camera bar
    learn_bar_state = !learn_bar_state;        // << set if the learning bar is visible or not
    set_learn_bar_visibility(learn_bar_state); // >>
    learn();                                   // and go into the learn mode
}

void MainWindow::toggle_camera_bar() // ditto for the camera bar
{
    set_learn_bar_visibility(HIDDEN);
    camera_bar_state = !camera_bar_state;
    set_camera_bar_visibility(camera_bar_state);
}

void MainWindow::set_camera_bar_visibility(bool state) // hide the camera bar, by setting height to 0
{
    camera_bar_state = state;
    ui->follow_blue->setVisible(camera_bar_state);
    ui->follow_red->setVisible(camera_bar_state);
    ui->follow_green->setVisible(camera_bar_state);
    ui->follow_stop->setVisible(camera_bar_state);
}

void MainWindow::set_learn_bar_visibility(bool state) // hide the learn bar, by setting the height to 0
{
    learn_bar_state = state;
    ui->execute->setVisible(learn_bar_state);
    ui->next->setVisible(learn_bar_state);
    ui->prev->setVisible(learn_bar_state);
}

void MainWindow::update_axes() // this updates the axes display
{
    float32_t *data = dev.servo_readall(); // read the values from all of the servos


    for (int i = 0; i < 6; i++)
    {
        switch (i + 1)
        {
        case 1:
        case 6:
            dev.angles[i] = data[i] - dev.home_position[i];
            break;
        case 2:
        case 3:
            dev.angles[i] = -(data[i] - dev.home_position[i]);
            dev.angles[i] = -(data[i] - dev.home_position[i]);
            break;

        case 4:
            dev.angles[i] = -data[i];
            break;
        
        case 5:
            dev.angles[i] = data[i] - dev.home_position[i];
            break;

        }

    }

    ui->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(dev.angles[0]).substr(0, 5) + "°").c_str()); // and set the strings for
    ui->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(dev.angles[1]).substr(0, 5) + "°").c_str()); // the labels
    ui->a3_d->setText(std::string(std::string("Axis 3: ") + std::to_string(dev.angles[2]).substr(0, 5) + "°").c_str());
    ui->a4_d->setText(std::string(std::string("Axis 4: ") + std::to_string(dev.angles[3]).substr(0, 5) + "°").c_str());
    ui->a5_d->setText(std::string(std::string("Axis 5: ") + std::to_string(dev.angles[4]).substr(0, 5) + "°").c_str());
    ui->a6_d->setText(std::string("Gripper: " + std::to_string(int(dev.angles[5])) + "%").c_str());

    float32_t end_effector[16];
    dev.calculate_end_effector(end_effector);

    ui->base_x->setText("X: " + QString(std::to_string(end_effector[12]).substr(0, 5).c_str()) + "mm");
    ui->base_y->setText("Y: " + QString(std::to_string(end_effector[13]).substr(0, 5).c_str()) + "mm");
    ui->base_z->setText("Z: " + QString(std::to_string(end_effector[14]).substr(0, 5).c_str()) + "mm");

    ui->base_Rx->setText("Rx: " + QString(std::to_string(atan(end_effector[5] / end_effector[8]) * degRad).substr(0, 5).c_str()) + "°");
    ui->base_Ry->setText("Ry: " + QString(std::to_string(asin(-end_effector[2]) * degRad).substr(0, 5).c_str()) + "°");
    ui->base_Rz->setText("Rz: " + QString(std::to_string(atan(end_effector[1] / end_effector[0]) * degRad).substr(0, 5).c_str()) + "°");

    delete data; // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}