#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <chrono>

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;



void MainWindow::updateViewfinderFrame()
{
    displaySync_.lock();
    QImage qt_image = QImage((const unsigned char *)(incomingFrame_.data), incomingFrame_.cols, incomingFrame_.rows, QImage::Format_RGB888);
    ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
    ui->viewfinder->updateGeometry();
}

void MainWindow::getFrame() // this is 2am code.
{                          // runs the viewfinder, alongside color detection

    int connected = true;

    while (applicationIsRunning_)
    {
        if ( postProcessinThread_.isFinished() )
        {
            postProcessinThread_ = QtConcurrent::run(this, &MainWindow::postprocessImage);
            Logger::Warning("Warning! Lost postprocessor. Restarted.");
        }
        
        int read_confirm = false;
        try
        {
            read_confirm = robotCamera_->read(incomingFrame_);
        }catch(std::exception e)
        {
            read_confirm = false;
        }

        if (incomingFrame_.empty() || !read_confirm)
        {
            Logger::Warning("Camera failure! Restarting...");
            connect(Scheduler_100ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
            return;
        }
        synchroMesh_.unlock();
    }
    return;
}

void MainWindow::postprocessImage()
{
    cv::Mat bw;

    while (applicationIsRunning_)
    {

        if(!cameraThread_.isRunning())
        {
            return;
        }
        synchroMesh_.lock();
            
        int w;
        int h;
        uint8_t *buf = quirc_begin(qrDecoder_, &w, &h);
        cvtColor(incomingFrame_, bw, COLOR_BGR2GRAY, 0);
        for (int_fast32_t y = 0; y < bw.rows; y++)
            for (int_fast32_t x = 0; x < bw.cols; x++)
                buf[(y * w + x)] = bw.at<uint8_t>(y, x);
        quirc_end(qrDecoder_);

        if (quirc_count(qrDecoder_) > 0)
        {
            struct quirc_code code;
            struct quirc_data data;

            quirc_extract(qrDecoder_, 0, &code);
            quirc_decode(&code, &data);
            std::string buffer((char *)&data.payload);
            uint16_t checksum = 0;
            for (const char &c : buffer)
            {
                if (c != ' ')
                {
                    checksum += c;
                }
                else
                {
                    if ((std::to_string(checksum) == buffer.substr(buffer.find_last_of(" ") + 1)))
                    {
                        switch (QMessageBox::question(
                            this,
                            tr("Open program?"),
                            tr(("Launch program " + buffer.substr(0, buffer.find(" ")) + " ?").c_str()),

                            QMessageBox::Yes |
                                QMessageBox::No |
                                QMessageBox::Cancel,

                            QMessageBox::Cancel))
                        {
                        case QMessageBox::Yes:
                            manualProgramStack_.clear();
                            // prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.homePosition_, manual_program); FIXME:
                            incomingFilename_ = QString(("./programs/" + buffer.substr(0, buffer.find(" ")) + ".bin").c_str());
                            break;
                        }
                        break;
                    }
                }
            }
        }

        Mat imgHSV;
        try
        {
            cvtColor(incomingFrame_, imgHSV, COLOR_BGR2HSV); // convert the image to HSV, or Hue Saturation Value
        }
        catch (cv::Exception e)
        {
            return;
        }

        line(incomingFrame_, Point(320, 220), Point(320, 260), CV_RGB(255, 0, 0), 1); // draw the crosshair
        line(incomingFrame_, Point(300, 240), Point(340, 240), CV_RGB(255, 0, 0), 1);
        // to hue saturation values, for easier processing
        Mat imgTreshRed;
        Mat imgTreshRed1;
        inRange(imgHSV, Scalar(0, 50, 50), Scalar(10, 255, 255), imgTreshRed); // we treshold the image, removing every color but red
        inRange(imgHSV, Scalar(170, 50, 50), Scalar(180, 255, 255), imgTreshRed1);
        imgTreshRed += imgTreshRed1;

        Mat imgTreshGreen;
        inRange(imgHSV, Scalar(45, 72, 92), Scalar(102, 255, 255), imgTreshGreen);

        Mat imgTreshBlue;
        inRange(imgHSV, Scalar(112, 60, 63), Scalar(124, 255, 255), imgTreshBlue);

        Mat res_red;                                     // by doing bitwise and with the treshold. anything that isn't
        bitwise_and(incomingFrame_, incomingFrame_, res_red, imgTreshRed); // red, green or blue automatically gets turned to 0 ( as a pixel )
                                                         // with bitwise and, they get destroyed
        Mat res_green;
        bitwise_and(incomingFrame_, incomingFrame_, res_green, imgTreshGreen);

        Mat res_blue;
        bitwise_and(incomingFrame_, incomingFrame_, res_blue, imgTreshBlue);

        std::vector<std::vector<Point>> contours;
        findContours(imgTreshRed, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        int max = 0;
        int ind = 0;

        for (auto &contour : contours)
            if (contourArea(contour) > max)
            {
                ind = &contour - &contours[0];
                max = contourArea(contour);
            }

        if (max > 0)
        {
            redBoundingBox_ = boundingRect(contours[ind]);
            rectangle(incomingFrame_, redBoundingBox_.tl(), redBoundingBox_.br(), CV_RGB(255, 0, 0), 3);
            putText(incomingFrame_, "Red", redBoundingBox_.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255));
        }

        contours.clear();
        findContours(imgTreshGreen, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        max = 0;
        for (auto &contour : contours)
            if (contourArea(contour) > max)
            {
                ind = &contour - &contours[0];
                max = contourArea(contour);
            }

        if (max > 50)
        {
            greenBoundingBox_ = boundingRect(contours[ind]);
            rectangle(incomingFrame_, greenBoundingBox_.tl(), greenBoundingBox_.br(), CV_RGB(0, 255, 0), 3);
            putText(incomingFrame_, "Green", greenBoundingBox_.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0));
        }

        contours.clear();
        findContours(imgTreshBlue, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        max = 0;
        for (auto &contour : contours)
            if (contourArea(contour) > max)
            {
                ind = &contour - &contours[0];
                max = contourArea(contour);
            }

        if (max > 50)
        {
            blueBoundingBox_ = boundingRect(contours[ind]);
            rectangle(incomingFrame_, blueBoundingBox_.tl(), blueBoundingBox_.br(), CV_RGB(0, 0, 255), 3);
            putText(incomingFrame_, "Blue", blueBoundingBox_.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0));
        }
        displaySync_.unlock();
    }
}

/*

        */