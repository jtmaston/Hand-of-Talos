#include "../lib/mainwindow.h"
#include "../lib/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);
    connect(ui -> learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));
    connect(ui -> track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar()));

    //robot.reset();

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::toggle_learn_bar()
{
    set_camera_bar_visibility(HIDDEN);
    learn_bar_state = !learn_bar_state;
    set_learn_bar_visibility(learn_bar_state);
}

void MainWindow::toggle_camera_bar()
{
    set_learn_bar_visibility(HIDDEN);
    camera_bar_state = !camera_bar_state;
    set_camera_bar_visibility(camera_bar_state);
}

void MainWindow::set_camera_bar_visibility(bool state)
{
    camera_bar_state = state;
    ui -> follow_blue -> setVisible(camera_bar_state);
    ui -> follow_red -> setVisible(camera_bar_state);
    ui -> follow_green -> setVisible(camera_bar_state);
    ui -> follow_stop -> setVisible(camera_bar_state);
}

void MainWindow::set_learn_bar_visibility(bool state)
{
    learn_bar_state = state;
    ui -> execute -> setVisible(learn_bar_state);
    ui -> next -> setVisible(learn_bar_state);
    ui -> prev -> setVisible(learn_bar_state);
}
