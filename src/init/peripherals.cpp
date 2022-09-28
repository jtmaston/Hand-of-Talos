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
        connect(Scheduler_100ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
    }
    else
    {
        cameraThread_ = QtConcurrent::run(this, &MainWindow::getFrame);
        //if(!postProcessinThread_.isRunning())
        //    postProcessinThread_ = QtConcurrent::run(this, &MainWindow::postprocessImage);
    }
}

bool MainWindow::detectJoystickHotplug()
{

    auto gamepads = QGamepadManager::instance()->connectedGamepads();
    if (gamepads.isEmpty())
    {
        ui->jog_btn->setEnabled(false);
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
    extern Image NoSignal;
    QImage qt_image = QImage((const unsigned char *)(NoSignal.pixel_data), NoSignal.width, NoSignal.height, QImage::Format_RGB888);

    if (incomingFrame_.empty())
        incomingFrame_ = cv::Mat(NoSignal.height, NoSignal.width, CV_8UC3, (unsigned char *)NoSignal.pixel_data);

    cv::resize(incomingFrame_, incomingFrame_, Size(751, 481));

    for (int i = 0; i < 10; i++)
    {
        if (robotCamera_->open(i, CAP_V4L2))
        {
            Logger::Info("Camera at /dev/video" + std::to_string(i) + " available");
            disconnect(Scheduler_100ms_, SIGNAL(timeout()), this, SLOT(cameraRestarter()));
            cameraThread_.waitForFinished();
            Logger::Info("Restarting!");
            Logger::Info("Waiting for process thread to die...");
            synchroMesh_.unlock();
            postProcessinThread_.waitForFinished();
            Logger::Info("Exited!");
            cameraThread_ = QtConcurrent::run(this, &MainWindow::getFrame);
            //if(!postProcessinThread_.isRunning())
            //    postProcessinThread_ = QtConcurrent::run(this, &MainWindow::postprocessImage);
            Logger::Info("Done launching threads");
            return;
        }
    }
}