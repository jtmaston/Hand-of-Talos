//
// Created by aanas on 12/12/22.
//

#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"
#include "inc/noSignal.hpp"

void MainWindow::cameraRestarter()
{

    QImage qt_image = QImage((NO_SIGNAL_pixel_data), NO_SIGNAL_WIDTH, NO_SIGNAL_HEIGHT, QImage::Format_RGB888);
    qt_image = qt_image.scaled(864, 480);
    ui_->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
    ui_->viewfinder->updateGeometry();
    for (int i = 0; i < 10; i++)
    {
        if (camera_->open(i, cv::CAP_V4L2))
        {
            camera_->set(cv::CAP_PROP_FRAME_WIDTH, 864);
            camera_->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
            camera_->set(cv::CAP_PROP_FPS, 30);

            disconnect(scheduler100Ms_, SIGNAL(timeout()), this, SLOT(cameraRestarter()));
            connect(scheduler16Ms_, SIGNAL(timeout()), SLOT(capture()));
            return;
        }
    }
}

void MainWindow::capture() // this is 2am code.
{                          // runs the viewfinder, alongside color detection

    int connected = true;

    cv::Mat frame;
    cv::Mat bw;
    connected = camera_->read(frame);
    if (frame.empty())
    {
        ////std::cout << "Suspected camera failure!";
        Logger::info("Lost camera!");
        connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
        disconnect(scheduler16Ms_, SIGNAL(timeout()), this, SLOT(capture()));
        return;
    }

    int w;
    int h;
    if(decoder_ != nullptr) {
        uint8_t *buf = quirc_begin(decoder_, &w, &h);
        cvtColor(frame, bw, cv::COLOR_BGR2GRAY, 0);
        for (int_fast32_t y = 0; y < bw.rows; y++)
            for (int_fast32_t x = 0; x < bw.cols; x++)
                buf[(y * w + x)] = bw.at<uint8_t>(y, x);
        quirc_end(decoder_);

        if (quirc_count(decoder_) > 0 && hasFinishedRunning_)
        {
            struct quirc_code code;
            struct quirc_data data;

            quirc_extract(decoder_, 0, &code);
            quirc_decode(&code, &data);
            std::string buffer ((char*)&data.payload);
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
                                tr("Open initializeInterpreterThread?"),
                                tr(("Launch initializeInterpreterThread " + buffer.substr(0, buffer.find(" ")) + " ?").c_str()),

                                QMessageBox::Yes |
                                QMessageBox::No |
                                QMessageBox::Cancel,

                                QMessageBox::Cancel))
                        {
                            case QMessageBox::Yes:
                                manualProgram_.clear();
                                //progThread_ = QtConcurrent::run(this, &MainWindow::rasmInterpreter, dev_.homePosition_, manualProgram_); TODO: FIXME: this
                                filename_ = QString(("./programs/" + buffer.substr(0, buffer.find(" ")) + ".bin").c_str());
                                hasFinishedRunning_ = false;
                                break;
                        }
                        break;
                    }
                }
            }
        }
    }

    cv::Mat imgHSV;
    try
    {
        cvtColor(frame, imgHSV, cv::COLOR_BGR2HSV); // convert the image to HSV, or Hue Saturation Value
    }
    catch (cv::Exception e)
    {
        return;
    }

    line(frame,
         cv::Point(frame.cols / 2, frame.rows / 2 - 20),
         cv::Point(frame.cols / 2, frame.rows / 2 + 20),
         CV_RGB(255, 0, 0),
         1);
    // draw the crosshair
    line(frame,
         cv::Point(frame.cols / 2 - 20, frame.rows / 2),
         cv::Point(frame.cols / 2 + 20, frame.rows / 2),
         CV_RGB(255, 0, 0),
         1);

    //line(frame, cv::Point(300, 240), cv::Point(340, 240), CV_RGB(255, 0, 0), 1);
    // to hue saturation values, for easier processing
    cv::Mat imgTreshRed;
    cv::Mat imgTreshRed1;
    inRange(imgHSV, cv::Scalar(0, 50, 50), cv::Scalar(10, 255, 255), imgTreshRed); // we treshold the image, removing every color but red
    inRange(imgHSV, cv::Scalar(170, 50, 50), cv::Scalar(180, 255, 255), imgTreshRed1);
    imgTreshRed += imgTreshRed1;

    cv::Mat imgTreshGreen;
    inRange(imgHSV, cv::Scalar(45, 72, 92), cv::Scalar(102, 255, 255), imgTreshGreen);

    cv::Mat imgTreshBlue;
    inRange(imgHSV, cv::Scalar(112, 60, 63), cv::Scalar(124, 255, 255), imgTreshBlue);

    cv::Mat res_red;                                     // by doing bitwise and with the treshold. anything that isn't
    bitwise_and(frame, frame, res_red, imgTreshRed); // red, green or blue autocv::Matically gets turned to 0 ( as a pixel )
    // with bitwise and, they get destroyed
    cv::Mat res_green;
    bitwise_and(frame, frame, res_green, imgTreshGreen);

    cv::Mat res_blue;
    bitwise_and(frame, frame, res_blue, imgTreshBlue);

    std::vector<std::vector<cv::Point>> contours;
    findContours(imgTreshRed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

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
        red_ = boundingRect(contours[ind]);
        rectangle(frame, red_.tl(), red_.br(), CV_RGB(255, 0, 0), 3);
        putText(frame, "Red", red_.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255));
    }

    contours.clear();
    findContours(imgTreshGreen, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    max = 0;
    for (auto &contour : contours)
        if (contourArea(contour) > max)
        {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 50)
    {
        green_ = boundingRect(contours[ind]);
        rectangle(frame, green_.tl(), green_.br(), CV_RGB(0, 255, 0), 3);
        putText(frame, "Green", green_.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0));
    }

    contours.clear();
    findContours(imgTreshBlue, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    max = 0;
    for (auto &contour : contours)
        if (contourArea(contour) > max)
        {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 50)
    {
        blue_ = boundingRect(contours[ind]);
        rectangle(frame, blue_.tl(), blue_.br(), CV_RGB(0, 0, 255), 3);
        putText(frame, "Blue", blue_.tl(), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 0, 0));
    }
    QImage qt_image = QImage((const unsigned char *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    // qt_image = qt_image.scaled(751, 481);
    ui_->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
    ui_->viewfinder->updateGeometry();
    // break;
    //}
    //}

    return;
}
cv::ErrorCallback camera_error_handler(int status, const char *func_name, const char *err_msg,
                                       const char *file_name, int line, void *) {

    return 0;
}