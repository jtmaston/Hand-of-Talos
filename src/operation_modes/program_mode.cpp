#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::runProgram() {
    manualProgramStack_.clear();



    //prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, manual_program);
    //FIXME:
}

void MainWindow::checkForFileIngress() {
    if (hasFileOpen_) {
        incomingFilename_ = QFileDialog::getOpenFileName(this,
                                                         "Pick the program file: ", "./",
                                                         "Compiled robot Assembly Files (*.bin)");

        if (incomingFilename_.size() == 0) {
            runningProgram_ = false;
            hasFileOpen_ = false;
        }
    }
}