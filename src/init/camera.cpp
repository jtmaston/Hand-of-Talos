#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

auto handleError(int status, [[maybe_unused]] const char *funcName,
                 [[maybe_unused]]  const char *errMsg, [[maybe_unused]] const char *fileName,
                 int line, void *userdata) -> int {

    return 0; // Return value is not used
}

void MainWindow::initCamera() {
    camera_ = new VideoCapture(0, CAP_V4L2);
    camera_->set(cv::CAP_PROP_FRAME_WIDTH, 864);
    camera_->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera_->set(cv::CAP_PROP_FPS, 30);
    decoder_ = quirc_new();
    quirc_resize(decoder_, 864, 480);

    if (!camera_->isOpened()) {
        connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
    } else {
        connect(scheduler16Ms_, SIGNAL(timeout()), SLOT(capture()));
    }
    cv::redirectError(handleError);
}