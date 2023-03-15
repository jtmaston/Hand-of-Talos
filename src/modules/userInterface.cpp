//
// Created by aanas on 12/12/22.
//

#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::setButtonColor(int index) {
    /*for(int i = 0 ; i < buttons_.size(); i++)
    {
        if(i == index) {
            buttons_.at(i)->setProperty("selectedActive", true);
            buttons_.at(i)->setStyleSheet(buttons_.at(i)->styleSheet());
        }
        else {
            buttons_.at(i)->setProperty("selectedActive", false);
            buttons_.at(i)->setStyleSheet(buttons_.at(i)->styleSheet());
        }
    }*/
}

void MainWindow::toggleLearnBar() // TODO: trigger to toggle the mode, not the bar
{
    //jogging_ = false;
    //setButtonColor(0);
    //setCameraBarVisibility(HIDDEN);         // hide the camera bar
    //learnBarState_ = !learnBarState_;        // << set if the learning bar is visible or not
    //setLearnBarVisibility(learnBarState_); // >>
    //learn();                                   // and go into the learn mode

}

void MainWindow::toggleCameraBar() // ditto for the camera bar
{
    jogging_ = false;
    setButtonColor(1);
    setLearnBarVisibility(HIDDEN);
    cameraBarState_ = !cameraBarState_;
    setCameraBarVisibility(cameraBarState_);

}

void MainWindow::setCameraBarVisibility(bool state) // hide the camera bar, by setting height to 0
{
    cameraBarState_ = state;
    ui_->follow_blue->setVisible(cameraBarState_);
    ui_->follow_red->setVisible(cameraBarState_);
    ui_->follow_green->setVisible(cameraBarState_);
    ui_->follow_stop->setVisible(cameraBarState_);
}

void MainWindow::setLearnBarVisibility(bool state) // hide the learn bar, by setting the height to 0
{
    learnBarState_ = state;
    ui_->learnModeBar->setVisible(learnBarState_);

}

void MainWindow::startFollowRed()
{
    switch (dir_)
    {
        case 0:
        {
            dir_ = 1;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(followColor()));
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    dir_ = 1;

    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 0};

    /*write(dev_.ledBus_, mode, 2);     // TODO: set rgb
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);*/
}

void MainWindow::startFollowGreen()
{
    switch (dir_)
    {
        case 0:
        {
            dir_ = 2;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(followColor()));
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    dir_ = 2;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 1};

    /*write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);*/
}

void MainWindow::startFollowBlue()
{
    switch (dir_)
    {
        case 0:
        {
            dir_ = 3;
            connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(followColor()));
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(command()));
            break;
        }
    }
    dir_ = 3;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 2};

    /*write(dev_.ledBus_, mode, 2);
    write(dev_.ledBus_, speed, 2);
    write(dev_.ledBus_, color, 2);*/
}

void MainWindow::stopFollow()
{
    //disconnect(scheduler500Ms_, SIGNAL(timeout()), this, SLOT(followColor()));
    //connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    //write(dev_.ledBus_, cmd, 2);
}

// Source: https://stackoverflow.com/a/16606128
#include <sstream>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 1)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}



// End source

cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R)
{

    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return cv::Vec3f(x, y, z);

}

typedef std::array<float, 3> float3;
typedef std::array<float3, 3> float3x3;

const float PI = 3.14159265358979323846264f;

void matrixToEuler(const cv::Mat& rotationMatrix, cv::Vec3f& eulerAngles)
{
    double sy = std::sqrt(rotationMatrix.at<double>(0, 0) * rotationMatrix.at<double>(0, 0) +
                          rotationMatrix.at<double>(1, 0) * rotationMatrix.at<double>(1, 0));
    bool singular = sy < 1e-6; // If |cos(y)| is close to zero, we have a singularity

    double x, y, z;
    if (!singular)
    {
        x = std::atan2(rotationMatrix.at<double>(2, 1), rotationMatrix.at<double>(2, 2));
        y = std::atan2(-rotationMatrix.at<double>(2, 0), sy);
        z = std::atan2(rotationMatrix.at<double>(1, 0), rotationMatrix.at<double>(0, 0));
    }
    else
    {
        x = std::atan2(-rotationMatrix.at<double>(1, 2), rotationMatrix.at<double>(1, 1));
        y = std::atan2(-rotationMatrix.at<double>(2, 0), sy);
        z = 0;
    }

    eulerAngles = cv::Vec3d(x, y, z);
}


void MainWindow::updateAxes() // this updates the axes display
{
    //std::array<float, 6> data = dev_.servoReadall(); // read the values from all of the servos
    //dev_.angles_.at(0) = 23;
    //dev_.angles_.at(2) = 15;
    /*for(int i = 0 ; i < 6; i++)
    {
        dev_.angles_.at(i) = data.at(i);
    }*/

    dev_.angles_.at(3) = 15;

    ui_->a1_d->setText(std::string(std::string("Axis 1: ") + to_string_with_precision(dev_.angles_[0])  +
                                   "°").c_str()); // and set the strings for
    ui_->a2_d->setText(std::string(
            std::string("Axis 2: ") + to_string_with_precision(dev_.angles_[1]) + "°").c_str()); // the labels
    ui_->a3_d->setText(
            std::string(std::string("Axis 3: ") + to_string_with_precision(dev_.angles_[2]) + "°").c_str());
    ui_->a4_d->setText(
            std::string(std::string("Axis 4: ") + to_string_with_precision(dev_.angles_[3]) + "°").c_str());
    ui_->a5_d->setText(
            std::string(std::string("Axis 5: ") + to_string_with_precision(dev_.angles_[4]) + "°").c_str());
    ui_->a6_d->setText(std::string("Gripper: " + to_string_with_precision(int(dev_.angles_[5])) + "%").c_str());

    std::array<float, 16> end_effector{};
    dev_.calculateEndEffector(end_effector);

    ui_->base_x->setText("X: " + QString(to_string_with_precision(end_effector[12]).c_str()) + "mm");
    ui_->base_y->setText("Y: " + QString(to_string_with_precision(end_effector[13]).c_str()) + "mm");
    ui_->base_z->setText("Z: " + QString(to_string_with_precision(end_effector[14]).c_str()) + "mm");


    cv::Mat ee = cv::Mat(4, 4, CV_32F, &end_effector);
    cv::transpose(ee, ee);
    cv::Mat rot = ee ( cv::Rect( 0, 0, 3, 3));


    //std::cout << format(rot, cv::Formatter::FMT_PYTHON) << std::endl<< std::endl<< std::endl<< std::endl;

    //auto a = rotationMatrixToEulerAngles(rot);


    cv::Vec3f result;
    matrixToEuler(rot, result);

    std::cout << result[0] * 57.2958 - 90 << " " << result[1]* 57.2958 << " " << result[2]* 57.2958 - 90;

    std::cout << '\n';


    ui_->base_Rx->setText(
            "Rx: " + QString(to_string_with_precision(atan(end_effector[5] / end_effector[8]) * DEG_RAD).c_str()) +
            "°");
    ui_->base_Ry->setText(
            "Ry: " + QString(to_string_with_precision(asin(-end_effector[2]) * DEG_RAD).c_str()) + "°");
    ui_->base_Rz->setText(
            "Rz: " + QString(to_string_with_precision(atan(end_effector[1] / end_effector[0]) * DEG_RAD).c_str()) +
            "°");

}

/*std::cout  << format(ee, cv::Formatter::FMT_PYTHON) << std::endl << std::endl
            << format(rot, cv::Formatter::FMT_PYTHON)
            << std::endl << std::endl
            << std::endl << std::endl ;*/

//auto a = rotationMatrixToEulerAngles()

void MainWindow::changeView(int a)
{
    buttons_.at( a - 1 )->setProperty("selectedActive", true);
    buttons_.at( a - 1 )->setStyleSheet(buttons_.at( a - 1 )->styleSheet());
    switch(a)
    {
        case 1:
        {
            toggleLearnBar();
            setLearnBarVisibility(true);
            learn();
            break;
        }
        case 2:
        {
            followTarget_.insert(followTarget_.begin(), &dev_.angles_[0], &dev_.angles_[dev_.angles_.size()]);
            toggleCameraBar();
            break;
        }
        case 3:
        {
            ui_->jogSliders->setVisible(true);
            break;
        }
        default:
            return;
    }
}

void MainWindow::changeMenu()
{

    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender()); // retrieve the button you have clicked
    std::map<std::string, int> mapper =
            {
                    {"learn_btn", 1},
                    {"track_btn", 2},
                    {"jog_btn", 3},
                    {"load_btn", 4},
                    {"halt_btn", 5}
            };


    qApp->exit(mapper.at(buttonSender->objectName().toStdString()));
}