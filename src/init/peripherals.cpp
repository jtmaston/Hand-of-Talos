#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::init_peripherals()
{
    camera = new VideoCapture(0, CAP_V4L2);
    camera->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera->set(cv::CAP_PROP_FPS, 30);

    decoder = quirc_new();
    quirc_resize(decoder, 640, 480);

    if (!camera->isOpened())
    {
        connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(camera_restarter()));
    }
    else
    {
        connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));
    }
}

bool MainWindow::joystick_hotplug_detect()
{

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty())
    {
        ui->jog_btn->setEnabled(false);
        //std::cout << "[ WARN ] No gamepad detected.\n";

        if (joystick != nullptr)
        {
            delete joystick;
            joystick = nullptr;
        }
        return false;
    }

    if (joystick != nullptr)
        return true;

    //std::cout << "[ INFO ] Joystick connected\n";

    joystick = new QGamepad(*gamepads.begin(), this);

    return true;
}

void MainWindow::camera_restarter()
{
    QImage qt_image = QImage((const unsigned char *)(NO_SIGNAL_pixel_data), NO_SIGNAL_WIDTH, NO_SIGNAL_HEIGHT, QImage::Format_RGB888);
    qt_image = qt_image.scaled(751, 481);
    ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
    ui->viewfinder->updateGeometry();
    for (int i = 0; i < 10; i++)
    {
        if (camera->open(i, CAP_V4L2))
        {
            // //std::cout << "Camera at /dev/video" << i << " available\n";
            disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(camera_restarter()));
            connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));
            return;
        }
    }
}