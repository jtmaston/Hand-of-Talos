/*
    The init_signals function connects all signals to their corresponding slots, essentially doing initialization of the Qt interface.
*/

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::crappyDelay(int ms)
{
    QTime die_time= QTime::currentTime().addSecs(ms / 1000);
    while (QTime::currentTime() < die_time)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}  

void MainWindow::initSignals()
{
    scheduler100Ms_ = new QTimer(this); // timer called every 100ms
    scheduler16Ms_ = new QTimer(this);  // timer called every  20ms
    scheduler500Ms_ = new QTimer(this);
    scheduler10Ms_ = new QTimer(this);

    connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(updateAxisDisplay()));            // axis readout is updated every 100ms
    //connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(moveToPosition()));                                    // control from the axis is also updated ever 100ms
    connect(scheduler100Ms_, SIGNAL(timeout()), SLOT(checkForFileIngress()));         // camera is updated every 20ms
    connect(scheduler10Ms_, SIGNAL(timeout()), SLOT(runGetCurrentPosition()));

    //ui_->base_r->setTracking(false);
    connect(ui_->base_r, SIGNAL(sliderReleased()), this, SLOT(moveToPosition()));

    connect(ui_->learn_btn, SIGNAL(clicked()), SLOT(testProcedure()));        // when the learn button is clicked, toggle the bar
    connect(ui_->track_btn, SIGNAL(clicked()), SLOT(toggleCameraBar()));       // when the track button is clicked, toggle the bar
    
    connect(ui_->add_step, SIGNAL(clicked()), SLOT(addStep()));                     //  <<
    connect(ui_->remove_step, SIGNAL(clicked()), SLOT(removeStep()));                  //      buttons for the learn mode
    connect(ui_->run_learned, SIGNAL(clicked()), SLOT(startFollowingPath()));               //  >>

    connect(ui_->follow_red, SIGNAL(clicked()), SLOT(setTrackingColorRed()));       //  <<
    connect(ui_->follow_green, SIGNAL(clicked()), SLOT(setTrackingColorGreen()));   //      buttons for color tracking mode
    connect(ui_->follow_blue, SIGNAL(clicked()), SLOT(setTrackingColorBlue()));
    connect(ui_->follow_stop, SIGNAL(clicked()), SLOT(stopTracking()));           //  >>

    connect(ui_->halt_btn, SIGNAL(clicked()), SLOT(emergencyStop()));                     // <<
    //connect(ui_->jog_btn, SIGNAL(clicked()), SLOT(jog()));                       //      top buttons
    connect(ui_->load_btn, SIGNAL(clicked()), SLOT(runProgram()));                  // >>


    //connect(ui_->base_r, SIGNAL(valueChanged(int)), ui_->increment_1, &QDoubleSpinBox::setValue);

    connect(ui_->base_r, &QSlider::valueChanged, ui_->increment_1, &QDoubleSpinBox::setValue);    //  <<
    connect(ui_->a2_r, &QSlider::valueChanged, ui_->increment_2, &QDoubleSpinBox::setValue);      //      Connect the sliders to the text boxes,
    connect(ui_->a3_r, &QSlider::valueChanged, ui_->increment_3, &QDoubleSpinBox::setValue);      //      so if one is updated so is the other
    connect(ui_->a4_r, &QSlider::valueChanged, ui_->increment_4, &QDoubleSpinBox::setValue);      //
    connect(ui_->a5_r, &QSlider::valueChanged, ui_->increment_5, &QDoubleSpinBox::setValue);      //
    connect(ui_->grip_r, &QSlider::valueChanged, ui_->increment_6, &QDoubleSpinBox::setValue);    //  >>

    connect(scheduler500Ms_, SIGNAL(timeout()), SLOT(detectJoystickHotplug()));               // start the hotplug detect of the joystick
    //connect(scheduler16Ms_, SIGNAL(timeout()), SLOT(updateViewfinderFrame()));
    //connect(scheduler10Ms_, SIGNAL(timeout()), SLOT(runCheckCollision()));

    scheduler100Ms_->start(100);
    scheduler16Ms_->start(33);
    scheduler500Ms_->start(500);
    scheduler10Ms_->start(10);
    
}