#include "inc/mainwindow.h"
#include "ui_mainwindow.h"

#include <QSlider>
#include <QDoubleSpinBox>

void MainWindow::setupApplication() {
    ui_->setupUi(this);

    initTimers();
    initButtons();
    initSliders();

    setCameraBarVisibility(HIDDEN);
    setLearnBarVisibility(HIDDEN);
}

void MainWindow::initTimers() {
    scheduler100Ms_ = new QTimer(this); // timer called every 100ms
    scheduler16Ms_ = new QTimer(this);  // timer called every  20ms
    scheduler500Ms_ = new QTimer(this);

    connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(updateAxes()));         // axis readout is updated every 100ms
    connect(scheduler100Ms_, SIGNAL(timeout()),
            SLOT(command()));             // control from the axis is also updated ever 100ms
    connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(checkIfFiledialog())); // camera is updated every 20ms
    connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(joystickHotplugDetect()));

    scheduler100Ms_->start(100);
    scheduler16Ms_->start(33);
    scheduler500Ms_->start(500);

}

void MainWindow::initButtons() {
    connect(ui_->learn_btn, SIGNAL(clicked()),
            SLOT(toggleLearnBar()));  // when the learn button is clicked, toggle the bar
    connect(ui_->track_btn, SIGNAL(clicked()),
            SLOT(toggleCameraBar())); // when the track button is clicked, toggle the bar

    //connect(ui_->next, SIGNAL(clicked()), SLOT(addStep()));           //  <<
    //connect(ui_->prev, SIGNAL(clicked()), SLOT(removeStep()));        //  buttons for the learn mode
    connect(ui_->execute, SIGNAL(clicked()), SLOT(followLearnedPath()));     //  >>
    connect(ui_->follow_stop, SIGNAL(clicked()), SLOT(stopFollow())); //  >>

    connect(ui_->follow_red, SIGNAL(clicked()), SLOT(startFollowRed()));
    connect(ui_->follow_green, SIGNAL(clicked()), SLOT(startFollowGreen()));
    connect(ui_->follow_blue, SIGNAL(clicked()), SLOT(startFollowBlue()));

    connect(ui_->halt_btn, SIGNAL(clicked()), SLOT(halt()));
    connect(ui_->jog_btn, SIGNAL(clicked()), SLOT(jog()));
    connect(ui_->load_btn, SIGNAL(clicked()), SLOT(program()));
}

void MainWindow::initSliders() const
{
    connect(ui_->base_r, &QSlider::valueChanged, ui_->increment_1, &QDoubleSpinBox::setValue);
    connect(ui_->a2_r, &QSlider::valueChanged, ui_->increment_2, &QDoubleSpinBox::setValue);
    connect(ui_->a3_r, &QSlider::valueChanged, ui_->increment_3, &QDoubleSpinBox::setValue);
    connect(ui_->a4_r, &QSlider::valueChanged, ui_->increment_4, &QDoubleSpinBox::setValue);
    connect(ui_->a5_r, &QSlider::valueChanged, ui_->increment_5, &QDoubleSpinBox::setValue);
    connect(ui_->grip_r, &QSlider::valueChanged, ui_->increment_6, &QDoubleSpinBox::setValue);

}