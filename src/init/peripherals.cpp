#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::initPeripherals()
{
    robotCamera_ = new VideoCapture(0, cv::CAP_V4L);
    robotCamera_->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    robotCamera_->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    robotCamera_->set(cv::CAP_PROP_FPS, 30  );

    qrDecoder_ = quirc_new();
    quirc_resize(qrDecoder_, 640, 480);

    synchroMesh_.lock();

    if (!robotCamera_->isOpened())
    {
        connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
    }
    else
    {
        cameraThread_ = QtConcurrent::run(this, &MainWindow::getFrame);
    }
}

auto MainWindow::detectJoystickHotplug() -> bool
{

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty())
    {
        ui_->jog_btn->setEnabled(false);
        // std::cout << "[ WARN ] No gamepad detected.\n";

        if (joystick_ != nullptr)
        {
            delete joystick_;
            joystick_ = nullptr;
        }
        return false;
    }

    if (joystick_ != nullptr)
        return true;

    // std::cout << "[ INFO ] Joystick connected\n";

    joystick_ = new QGamepad(*gamepads.begin(), this);

    return true;
}

void MainWindow::cameraRestarter()
{
    extern Image noSignal;
    // QImage qt_image = QImage((const unsigned char *)(noSignal.pixelData_), noSignal.width_, noSignal.height_, QImage::Format_RGB888);

    if (incomingFrame_.empty())
        incomingFrame_ = cv::Mat(noSignal.height_, noSignal.width_, CV_8UC3, (unsigned char *)noSignal.pixelData_);

    cv::resize(incomingFrame_, incomingFrame_, Size(751, 481));

    for (int i = 0; i < 10; i++)
    {
        if (robotCamera_->open(i, CAP_V4L2))
        {
            Logger::info("Camera at /dev/video" + std::to_string(i) + " available");
            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(cameraRestarter()));
            cameraThread_.waitForFinished();
            Logger::info("Restarting!");
            Logger::info("Waiting for process thread to die...");
            synchroMesh_.unlock();
            Logger::info("Exited!");
            cameraThread_ = QtConcurrent::run(this, &MainWindow::getFrame);
            Logger::info("Done launching threads");
            return;
        }
    }
}