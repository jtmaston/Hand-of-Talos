#include "inc/mainwindow.hpp"
#include "ui_mainwindow.h"

auto handleError([[maybe_unused]]int status, [[maybe_unused]] const char *funcName,
                 [[maybe_unused]]  const char *errMsg, [[maybe_unused]] const char *fileName,
                 [[maybe_unused]]int line,[[maybe_unused]] void *userdata) -> int {

    return 0; // Return value is not used
}

bool MainWindow::initCamera() {

    camera_ = new cv::VideoCapture(0, cv::CAP_V4L2);
    bool returnCode = (camera_ != nullptr);
    returnCode = returnCode && camera_->set(cv::CAP_PROP_FRAME_WIDTH, 864.0F);
    returnCode = returnCode && camera_->set(cv::CAP_PROP_FRAME_HEIGHT, 480.0F);
    returnCode = returnCode && camera_->set(cv::CAP_PROP_FPS, 30.0F);
    decoder_ = quirc_new();
    returnCode = returnCode && (quirc_resize(decoder_, 864, 480) == 0);

    if (!camera_->isOpened()) {
        (void) connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(cameraRestarter()));
    } else {
        (void) connect(scheduler16Ms_, SIGNAL(timeout()), SLOT(capture()));
    }
    (void) cv::redirectError(handleError);
    return returnCode;
}