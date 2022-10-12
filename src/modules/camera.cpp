#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <chrono>

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;


std::chrono::time_point<std::chrono::system_clock> t;

void MainWindow::updateViewfinderFrame() {
    //std::cout << duration_cast<milliseconds>((high_resolution_clock::now() - t)).count() << '\n';
    t = high_resolution_clock::now();
    //displaySync_.lock();
    QImage qt_image = QImage((const unsigned char *) (incomingFrame_.data), incomingFrame_.cols, incomingFrame_.rows,
                             QImage::Format_RGB888);
    ui_->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
    ui_->viewfinder->updateGeometry();
}

void MainWindow::getFrame() // this is 2am code.
{                          // runs the viewfinder, alongside color detection

bool read_confirm = false;
    while (applicationIsRunning_) {

        try {
            read_confirm = robotCamera_->read(incomingFrame_);
        } catch (std::exception& e) {
            read_confirm = false;
        }

        if (incomingFrame_.empty() || !read_confirm) {
            Logger::warning("Camera failure! Restarting...");
            connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
            return;
        }
        //postprocessImage();

        synchroMesh_.unlock();
        postprocessImage();
        updateViewfinderFrame();



    }
}

void MainWindow::postprocessImage() {
    using namespace std::chrono;

    // Use auto keyword to avoid typing long
    // type definitions to get the timepoint
    // at this instant use function now()


    cv::Mat bw;

    //while (true)
    //{

    if (!cameraThread_.isRunning()) {
        return;
    }
    //synchroMesh_.lock();

    int w = 0; int h = 0;
    uint8_t *buf = quirc_begin(qrDecoder_, &w, &h);
    cvtColor(incomingFrame_, bw, COLOR_BGR2GRAY, 0);
    for (int_fast16_t y = 0; y < bw.rows; y++)
        for (int_fast16_t x = 0; x < bw.cols; x++)
            buf[(y * w + x)] = bw.at<uint8_t>(y, x);
    quirc_end(qrDecoder_);

    if (quirc_count(qrDecoder_) > 0) {
        quirc_code code = {};
        quirc_data data = {};

        quirc_extract(qrDecoder_, 0, &code);
        quirc_decode(&code, &data);
        std::string buffer((char *) &data.payload);
        uint16_t checksum = 0;
        for (const char &c: buffer) {
            if (c != ' ') {
                checksum += c;
            } else {
                if ((std::to_string(checksum) == buffer.substr(buffer.find_last_of(' ') + 1))) {
                    switch (QMessageBox::question(
                            this,
                            tr("Open program?"),
                            tr(("Launch program " + buffer.substr(0, buffer.find(' ')) + " ?").c_str()),

                            QMessageBox::Yes |
                            QMessageBox::No |
                            QMessageBox::Cancel,

                            QMessageBox::Cancel)) {
                        case QMessageBox::Yes:
                            manualProgramStack_.clear();
                            // prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.homePosition_, manual_program); FIXME:
                            incomingFilename_ = QString(
                                    ("./programs/" + buffer.substr(0, buffer.find(' ')) + ".bin").c_str());
                            break;
                        default:
                            break;
                    }
                    break;
                }
            }
        }
    }

    Mat img_hsv;
    try {
        cvtColor(incomingFrame_, img_hsv, COLOR_BGR2HSV); // convert the image to HSV, or Hue Saturation Value
    }
    catch (cv::Exception &e) {
        return;
    }

    line(incomingFrame_, Point(320, 220), Point(320, 260), CV_RGB(255, 0, 0), 1); // draw the crosshair
    line(incomingFrame_, Point(300, 240), Point(340, 240), CV_RGB(255, 0, 0), 1);
    // to hue saturation values, for easier processing
    Mat img_threshold_red_0;
    Mat img_threshold_red_1;
    inRange(img_hsv, Scalar(0, 50, 50), Scalar(10, 255, 255),
            img_threshold_red_0); // we threshold the image, removing every color but red

    inRange(img_hsv, Scalar(170, 50, 50), Scalar(180, 255, 255), img_threshold_red_1);
    img_threshold_red_0 += img_threshold_red_1;

    Mat img_threshold_green;
    inRange(img_hsv, Scalar(45, 72, 92), Scalar(102, 255, 255), img_threshold_green);

    Mat img_threshold_blue;
    inRange(img_hsv, Scalar(112, 60, 63), Scalar(124, 255, 255), img_threshold_blue);

    Mat res_red;                                     // by doing bitwise and with the threshold. anything that isn't
    bitwise_and(incomingFrame_, incomingFrame_, res_red,
                img_threshold_red_0); // red, green or blue automatically gets turned to 0 ( as a pixel )
    // with bitwise and, they get destroyed
    Mat res_green;
    bitwise_and(incomingFrame_, incomingFrame_, res_green, img_threshold_green);

    Mat res_blue;
    bitwise_and(incomingFrame_, incomingFrame_, res_blue, img_threshold_blue);

    std::vector<std::vector<Point>> contours;
    findContours(img_threshold_red_0, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    double max = 0;
    long ind = 0;

    for (auto &contour: contours)
        if (contourArea(contour) > max) {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 0) {
        redBoundingBox_ = boundingRect(contours[ind]);
        rectangle(incomingFrame_, redBoundingBox_.tl(), redBoundingBox_.br(), CV_RGB(255, 0, 0), 3);
        putText(incomingFrame_, "Red", redBoundingBox_.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255));
    }

    contours.clear();
    findContours(img_threshold_green, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    max = 0;
    for (auto &contour: contours)
        if (contourArea(contour) > max) {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 50) {
        greenBoundingBox_ = boundingRect(contours[ind]);
        rectangle(incomingFrame_, greenBoundingBox_.tl(), greenBoundingBox_.br(), CV_RGB(0, 255, 0), 3);
        putText(incomingFrame_, "Green", greenBoundingBox_.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0));
    }

    contours.clear();
    findContours(img_threshold_blue, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    max = 0;
    for (auto &contour: contours)
        if (contourArea(contour) > max) {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 50) {
        blueBoundingBox_ = boundingRect(contours[ind]);
        rectangle(incomingFrame_, blueBoundingBox_.tl(), blueBoundingBox_.br(), CV_RGB(0, 0, 255), 3);
        putText(incomingFrame_, "Blue", blueBoundingBox_.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0));
    }
    displaySync_.unlock();
    //}
}

/*

        */