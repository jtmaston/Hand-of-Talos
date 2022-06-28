#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::program()
{
    manual_program.clear();

    

    //prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, manual_program);
    //FIXME:
}

void MainWindow::check_if_filedialog()
{
    if (fileopen)
    {
        filename = QFileDialog::getOpenFileName(this,
                                                "Pick the program file: ", "./", "Compiled robot Assembly Files (*.bin)");

        if (filename.size() == 0)
        {
            following_program = false;
            fileopen = false;
        }
    }
}