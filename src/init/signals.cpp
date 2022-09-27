/*
    The init_signals function connects all signals to their corresponding slots, essentially doing initialization of the Qt interface.
*/

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::crappyDelay(int ms)
{
    QTime dieTime= QTime::currentTime().addSecs(ms / 1000);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}  

void MainWindow::initSignals()
{
    Scheduler_100ms_ = new QTimer(this); // timer called every 100ms
    Scheduler_16ms_ = new QTimer(this);  // timer called every  20ms
    Scheduler_500ms_ = new QTimer(this);
    Scheduler_10ms_ = new QTimer(this);

    connect(Scheduler_100ms_, SIGNAL(timeout()), SLOT(updateAxisDisplay()));           // axis readout is updated every 100ms
    connect(Scheduler_500ms_, SIGNAL(timeout()), SLOT(moveToPosition()));               // control from the axis is also updated ever 100ms
    connect(Scheduler_100ms_, SIGNAL(timeout()), SLOT(checkForFileIngress()));   // camera is updated every 20ms
    connect(Scheduler_10ms_, SIGNAL(timeout()), SLOT(runGetCurrentPosition()));

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggleLearnBar()));        // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggleCameraBar()));       // when the track button is clicked, toggle the bar
    
    connect(ui->add_step, SIGNAL(clicked()), SLOT(addStep()));                     //  <<
    connect(ui->remove_step, SIGNAL(clicked()), SLOT(removeStep()));                  //      buttons for the learn mode
    connect(ui->run_learned, SIGNAL(clicked()), SLOT(startFollowingPath()));               //  >>

    connect(ui->follow_red, SIGNAL(clicked()), SLOT(setTrackingColorRed()));       //  <<
    connect(ui->follow_green, SIGNAL(clicked()), SLOT(setTrackingColorGreen()));   //      buttons for color tracking mode
    connect(ui->follow_blue, SIGNAL(clicked()), SLOT(setTrackingColorBlue()));
    connect(ui->follow_stop, SIGNAL(clicked()), SLOT(stopTracking()));           //  >>

    connect(ui->halt_btn, SIGNAL(clicked()), SLOT(emergencyStop()));                     // << 
    //connect(ui->jog_btn, SIGNAL(clicked()), SLOT(jog()));                       //      top buttons
    connect(ui->load_btn, SIGNAL(clicked()), SLOT(runProgram()));                  // >>

    connect(ui->factor_slider, &QSlider::valueChanged, ui->factor_box, &QSpinBox::setValue);

    connect(ui->base_r, &QSlider::valueChanged, ui->increment_1, &QDoubleSpinBox::setValue);    //  <<
    connect(ui->a2_r, &QSlider::valueChanged, ui->increment_2, &QDoubleSpinBox::setValue);      //      Connect the sliders to the text boxes,
    connect(ui->a3_r, &QSlider::valueChanged, ui->increment_3, &QDoubleSpinBox::setValue);      //      so if one is updated so is the other
    connect(ui->a4_r, &QSlider::valueChanged, ui->increment_4, &QDoubleSpinBox::setValue);      //
    connect(ui->a5_r, &QSlider::valueChanged, ui->increment_5, &QDoubleSpinBox::setValue);      //
    connect(ui->grip_r, &QSlider::valueChanged, ui->increment_6, &QDoubleSpinBox::setValue);    //  >>

    connect(Scheduler_500ms_, SIGNAL(timeout()), SLOT(detectJoystickHotplug()));               // start the hotplug detect of the joystick
    connect(Scheduler_16ms_, SIGNAL(timeout()), SLOT(updateViewfinderFrame()));
    connect(Scheduler_10ms_, SIGNAL(timeout()), SLOT(runCheckCollision()));

    Scheduler_100ms_->start(100);
    Scheduler_16ms_->start(33);
    Scheduler_500ms_->start(500);
    Scheduler_10ms_->start(10);
    
}