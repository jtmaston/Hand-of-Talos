/*
    The init_signals function connects all signals to their corresponding slots, essentially doing initialization of the Qt interface.
*/

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

void MainWindow::init_signals()
{
    Scheduler_100ms = new QTimer(this); // timer called every 100ms
    Scheduler_16ms = new QTimer(this);  // timer called every  20ms
    Scheduler_500ms = new QTimer(this);

    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_axes()));           // axis readout is updated every 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));               // control from the axis is also updated ever 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(check_if_filedialog()));   // camera is updated every 20ms

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));        // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar()));       // when the track button is clicked, toggle the bar

    connect(ui->next, SIGNAL(clicked()), SLOT(add_step()));                     //  <<
    connect(ui->prev, SIGNAL(clicked()), SLOT(remove_step()));                  //      buttons for the learn mode
    connect(ui->execute, SIGNAL(clicked()), SLOT(follow_path()));               //  >>
    

    connect(ui->follow_red, SIGNAL(clicked()), SLOT(start_follow_red()));       //  <<
    connect(ui->follow_green, SIGNAL(clicked()), SLOT(start_follow_green()));   //      buttons for color tracking mode
    connect(ui->follow_blue, SIGNAL(clicked()), SLOT(start_follow_blue()));
    connect(ui->follow_stop, SIGNAL(clicked()), SLOT(stop_follow()));           //  >>

    connect(ui->halt_btn, SIGNAL(clicked()), SLOT(halt()));                     // << 
    connect(ui->jog_btn, SIGNAL(clicked()), SLOT(jog()));                       //      top buttons
    connect(ui->load_btn, SIGNAL(clicked()), SLOT(program()));                  // >>

    connect(ui->base_r, &QSlider::valueChanged, ui->increment_1, &QDoubleSpinBox::setValue);    //  <<
    connect(ui->a2_r, &QSlider::valueChanged, ui->increment_2, &QDoubleSpinBox::setValue);      //      Connect the sliders to the text boxes,
    connect(ui->a3_r, &QSlider::valueChanged, ui->increment_3, &QDoubleSpinBox::setValue);      //      so if one is updated so is the other
    connect(ui->a4_r, &QSlider::valueChanged, ui->increment_4, &QDoubleSpinBox::setValue);      //
    connect(ui->a5_r, &QSlider::valueChanged, ui->increment_5, &QDoubleSpinBox::setValue);      //
    connect(ui->grip_r, &QSlider::valueChanged, ui->increment_6, &QDoubleSpinBox::setValue);    //  >>

    connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(joystick_hotplug_detect()));               // start the hotplug detect of the joystick

    Scheduler_100ms->start(100);
    Scheduler_16ms->start(33);
    Scheduler_500ms->start(500);
    
}