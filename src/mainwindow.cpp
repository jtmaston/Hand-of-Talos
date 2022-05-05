#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <chrono>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);

    Scheduler_100ms = new QTimer(this); // timer called every 100ms
    Scheduler_16ms = new QTimer(this);  // timer called every  20ms
    Scheduler_500ms = new QTimer(this);

    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_axes()));         // axis readout is updated every 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));             // control from the axis is also updated ever 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(check_if_filedialog())); // camera is updated every 20ms
    connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));  // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar())); // when the track button is clicked, toggle the bar

    connect(ui->next, SIGNAL(clicked()), SLOT(add_step()));           //  <<
    connect(ui->prev, SIGNAL(clicked()), SLOT(remove_step()));        //  buttons for the learn mode
    connect(ui->execute, SIGNAL(clicked()), SLOT(follow_path()));     //  >>
    connect(ui->follow_stop, SIGNAL(clicked()), SLOT(stop_follow())); //  >>

    connect(ui->follow_red, SIGNAL(clicked()), SLOT(start_follow_red()));
    connect(ui->follow_green, SIGNAL(clicked()), SLOT(start_follow_green()));
    connect(ui->follow_blue, SIGNAL(clicked()), SLOT(start_follow_blue()));

    connect(ui->halt_btn, SIGNAL(clicked()), SLOT(halt()));
    connect(ui->jog_btn, SIGNAL(clicked()), SLOT(jog()));
    connect(ui->load_btn, SIGNAL(clicked()), SLOT(program()));

    Scheduler_100ms->start(100);
    Scheduler_16ms->start(16);
    Scheduler_500ms->start(500);

    ui->a4_r->setValue(-90);
    running = true;

    connect(ui->base_r, &QSlider::valueChanged, ui->increment_1, &QDoubleSpinBox::setValue);
    connect(ui->a2_r, &QSlider::valueChanged, ui->increment_2, &QDoubleSpinBox::setValue);
    connect(ui->a3_r, &QSlider::valueChanged, ui->increment_3, &QDoubleSpinBox::setValue);
    connect(ui->a4_r, &QSlider::valueChanged, ui->increment_4, &QDoubleSpinBox::setValue);
    connect(ui->a5_r, &QSlider::valueChanged, ui->increment_5, &QDoubleSpinBox::setValue);
    connect(ui->grip_r, &QSlider::valueChanged, ui->increment_6, &QDoubleSpinBox::setValue);

    camera = new VideoCapture(0, CAP_V4L2);
    camera->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera->set(cv::CAP_PROP_FPS, 60);

    dev.toggleTorque(true);
}

MainWindow::~MainWindow()
{
    running = false;
    cam_thread.waitForFinished();
    joy_thread.waitForFinished();

    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
    //delete joystick;
    delete ui;
}
void MainWindow::toggle_learn_bar() // toggle the learning bar,
{
    set_camera_bar_visibility(HIDDEN);         // hide the camera bar
    learn_bar_state = !learn_bar_state;        // << set if the learning bar is visible or not
    set_learn_bar_visibility(learn_bar_state); // >>
    learn();                                   // and go into the learn mode
}

void MainWindow::toggle_camera_bar() // ditto for the camera bar
{
    set_learn_bar_visibility(HIDDEN);
    camera_bar_state = !camera_bar_state;
    set_camera_bar_visibility(camera_bar_state);
}

void MainWindow::set_camera_bar_visibility(bool state) // hide the camera bar, by setting height to 0
{
    camera_bar_state = state;
    ui->follow_blue->setVisible(camera_bar_state);
    ui->follow_red->setVisible(camera_bar_state);
    ui->follow_green->setVisible(camera_bar_state);
    ui->follow_stop->setVisible(camera_bar_state);
}

void MainWindow::set_learn_bar_visibility(bool state) // hide the learn bar, by setting the height to 0
{
    learn_bar_state = state;
    ui->execute->setVisible(learn_bar_state);
    ui->next->setVisible(learn_bar_state);
    ui->prev->setVisible(learn_bar_state);
}

void MainWindow::update_axes() // this updates the axes display
{
    float32_t *data = dev.servo_readall(); // read the values from all of the servos
    //dev.angles = data;
    dev.angles.reserve(7);                                  // TODO: prevent this preallocation from happening on each call
    memcpy(dev.angles.data(), data, 6 * sizeof(float32_t));

    ui->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(-(data[0] - dev.home_position[0])) + "°").c_str()); // and set the strings for
    ui->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(-(data[1] - dev.home_position[1])) + "°").c_str()); // the labels
    ui->a3_d->setText(std::string(std::string("Axis 3: ") + std::to_string(-(data[2] - dev.home_position[2])) + "°").c_str());
    ui->a4_d->setText(std::string(std::string("Axis 4: ") + std::to_string((data[3] - dev.home_position[3] + 170)) + "°").c_str());
    ui->a5_d->setText(std::string(std::string("Axis 5: ") + std::to_string(data[4] - dev.home_position[4]) + "°").c_str());
    ui->a6_d->setText(std::string("Axis 6: " + std::to_string(data[5] - dev.home_position[5]) + "°").c_str());

    float32_t end_effector[16];
    dev.calculate_end_effector(end_effector);

    ui->DK_X->setText(QString(std::to_string(end_effector[12]).c_str()));
    ui->DK_Y->setText(QString(std::to_string(end_effector[13]).c_str()));
    ui->DK_Z->setText(QString(std::to_string(end_effector[14]).c_str()));

    delete data; // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}

void MainWindow::command() // get the values from the sliders, then write them on the bus
{
    float32_t angles[6];
    angles[0] = ui->increment_1->value() + dev.home_position[0]; // need to adjust with 90
    angles[1] = ui->increment_2->value() + dev.home_position[1];
    angles[2] = ui->increment_3->value() + dev.home_position[2];
    angles[3] = ui->increment_4->value() + dev.home_position[3];
    angles[4] = ui->increment_5->value() + dev.home_position[4];
    angles[5] = ui->increment_6->value() + dev.home_position[5];

    ui->base_r->setValue(static_cast<int>(ui->increment_1->value()));
    ui->a2_r->setValue(static_cast<int>(ui->increment_2->value()));
    ui->a3_r->setValue(static_cast<int>(ui->increment_3->value()));
    ui->a4_r->setValue(static_cast<int>(ui->increment_4->value()));
    ui->a5_r->setValue(static_cast<int>(ui->increment_5->value()));
    ui->grip_r->setValue(static_cast<int>(ui->increment_6->value()));

    dev.servo_write6(angles, time_mod); // move the axes
}

void MainWindow::learn() // starts the learn mode
{
    //disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command())); // stop the control function
    //go_home();                                                             // move back to home
    dev.toggleTorque(false);                                               // and disable the torque
    learning = true;
}

void MainWindow::add_step() // add a step
{
    //float32_t *angle = dev.servo_readall(); // read all the servo values
    std::vector<float32_t> t(6);            // make them into a vector


    t [0] = -(dev.angles[0] - dev.home_position[0]);
    t [1] = -(dev.angles[1] - dev.home_position[1]);
    t [2] = -(dev.angles[2] - dev.home_position[2]);
    t [3] = (dev.angles[3] - dev.home_position[3]) + 170;
    t [4] = (dev.angles[4] - dev.home_position[4]);
    t [5] = (dev.angles[5] - dev.home_position[5]);

    

    Instruction local;
    local.opcode = TGT;
    local.params[0] = manual_program.size();
    memmove(local.params + 1, t.data(), 5 * sizeof(float32_t));
    manual_program.push_back(local);

    local.opcode = ANGS;
    local.params[0] = local.params[0];
    manual_program.push_back(local);
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str())); // then update the label
    //delete angle;
}

void MainWindow::remove_step() // remove a step from the queue
{
    dev.learned_angles.pop_back();
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str())); // then update the label
}

void MainWindow::run_learnprog()
{
    std::cout << dev.executing << '\n';
    following_program = true;
    toggle_jog();
    uint16_t stepcount = dev.learned_angles.size();
    // std::cout << stepcount << '\n';
    //std::cout.flush();
    while ( dev.executing )
    {
        for (int i = 0; i < stepcount && dev.executing; i++)
        {
            std::cout << "here";
            ui->increment_1->setValue(dev.learned_angles[i][1]);
            ui->increment_1->setValue(dev.learned_angles[i][2]);
            ui->increment_1->setValue(dev.learned_angles[i][3]);
            ui->increment_1->setValue(dev.learned_angles[i][4]);
            ui->increment_1->setValue(dev.learned_angles[i][5]);
            ui->increment_1->setValue(dev.learned_angles[i][6]);

            usleep(1000000);
        }
    }
}

void MainWindow::follow_path() // start running
{
    following_program = !following_program;
    prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, manual_program);

}

void MainWindow::capture() // this is 2am code.
{                          // runs the viewfinder, alongside color detection

    int connected = true;
    bool cap = 0;

    Mat frame;
    connected = camera->read(frame);

    Mat imgHSV;
    try
    {
        cvtColor(frame, imgHSV, COLOR_BGR2HSV); // convert the image to HSV, or Hue Saturation Value
    }
    catch (cv::Exception e)
    {
        return;
    }

    line(frame, Point(320, 0), Point(320, 480), CV_RGB(255, 0, 0), 1); // draw the crosshair
    line(frame, Point(0, 240), Point(640, 240), CV_RGB(255, 0, 0), 1);
    // to hue saturation values, for easier processing
    Mat imgTreshRed;
    Mat imgTreshRed1;
    inRange(imgHSV, Scalar(0, 50, 50), Scalar(10, 255, 255), imgTreshRed); // we treshold the image, removing every color but red
    inRange(imgHSV, Scalar(170, 50, 50), Scalar(180, 255, 255), imgTreshRed1);
    imgTreshRed += imgTreshRed1;

    Mat imgTreshGreen;
    inRange(imgHSV, Scalar(45, 72, 92), Scalar(102, 255, 255), imgTreshGreen);

    Mat imgTreshBlue;
    inRange(imgHSV, Scalar(112, 60, 63), Scalar(124, 255, 255), imgTreshBlue);

    Mat res_red;                                     // by doing bitwise and with the treshold. anything that isn't
    bitwise_and(frame, frame, res_red, imgTreshRed); // red, green or blue automatically gets turned to 0 ( as a pixel )
                                                     // with bitwise and, they get destroyed
    Mat res_green;
    bitwise_and(frame, frame, res_green, imgTreshGreen);

    Mat res_blue;
    bitwise_and(frame, frame, res_blue, imgTreshBlue);

    std::vector<std::vector<Point>> contours;
    findContours(imgTreshRed, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    int max = 0;
    int ind = 0;

    for (auto &contour : contours)
        if (contourArea(contour) > max)
        {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 0)
    {
        red = boundingRect(contours[ind]);
        rectangle(frame, red.tl(), red.br(), CV_RGB(255, 0, 0), 3);
        putText(frame, "Red", red.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255));
    }

    contours.clear();
    findContours(imgTreshGreen, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    max = 0;
    for (auto &contour : contours)
        if (contourArea(contour) > max)
        {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 50)
    {
        green = boundingRect(contours[ind]);
        rectangle(frame, green.tl(), green.br(), CV_RGB(0, 255, 0), 3);
        putText(frame, "Green", green.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0));
    }

    contours.clear();
    findContours(imgTreshBlue, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    max = 0;
    for (auto &contour : contours)
        if (contourArea(contour) > max)
        {
            ind = &contour - &contours[0];
            max = contourArea(contour);
        }

    if (max > 50)
    {
        blue = boundingRect(contours[ind]);
        rectangle(frame, blue.tl(), blue.br(), CV_RGB(0, 0, 255), 3);
        putText(frame, "Blue", blue.tl(), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0));
    }
    QImage qt_image = QImage((const unsigned char *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    qt_image = qt_image.scaled(751, 481);
    ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
    ui->viewfinder->updateGeometry();
    return;
}

void MainWindow::halt()
{
    dev.toggleTorque(false);
}

void MainWindow::start_follow_red()
{
    switch (dir)
    {
    case 0:
    {
        dir = 1;
        connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
        break;
    }
    }
    dir = 1;

    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 0};

    write(dev.led_bus, mode, 2);
    write(dev.led_bus, speed, 2);
    write(dev.led_bus, color, 2);
}

void MainWindow::start_follow_green()
{
    switch (dir)
    {
    case 0:
    {
        dir = 2;
        connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
        break;
    }
    }
    dir = 2;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 1};

    write(dev.led_bus, mode, 2);
    write(dev.led_bus, speed, 2);
    write(dev.led_bus, color, 2);
}

void MainWindow::start_follow_blue()
{
    switch (dir)
    {
    case 0:
    {
        dir = 3;
        connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
        break;
    }
    }
    dir = 3;
    uint8_t mode[] = {0x04, 1};
    uint8_t speed[] = {0x05, 3};
    uint8_t color[] = {0x06, 2};

    write(dev.led_bus, mode, 2);
    write(dev.led_bus, speed, 2);
    write(dev.led_bus, color, 2);
}

void MainWindow::stop_follow()
{
    disconnect(Scheduler_500ms, SIGNAL(timeout()), this, SLOT(follow()));
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
}

void MainWindow::follow()
{
    int x_center;
    int y_center;

    switch (dir)
    {
    case 1:
    {
        x_center = red.x + red.width / 2;
        y_center = red.y + red.y / 2;
        break;
    }
    case 2:
    {
        x_center = green.x + green.width / 2;
        y_center = green.y + green.y / 2;
        break;
    }
    case 3:
    {
        x_center = blue.x + blue.width / 2;
        y_center = blue.y + blue.y / 2;
        break;
    }
    }
    float32_t alpha = (asin((320 - x_center) / ((22) * pixMod)) * 180) / __PI__;
    float32_t beta = (asin((240 - y_center) / ((36) * pixMod)) * 180) / __PI__;

    ui->base_r->setValue(ui->base_r->value() + round(alpha));
    ui->a4_r->setValue(ui->a4_r->value() - round(beta));

    float32_t angles[6];
    angles[0] = ui->increment_1->value() + dev.home_position[0]; // need to adjust with 90
    angles[1] = ui->increment_2->value() + dev.home_position[1];
    angles[2] = ui->increment_3->value() + dev.home_position[2];
    angles[3] = ui->increment_4->value() + dev.home_position[3];
    angles[4] = ui->increment_5->value() + dev.home_position[4];
    angles[5] = ui->increment_6->value() + dev.home_position[5];

    ui->base_r->setValue(static_cast<int>(ui->increment_1->value()));
    ui->a2_r->setValue(static_cast<int>(ui->increment_2->value()));
    ui->a3_r->setValue(static_cast<int>(ui->increment_3->value()));
    ui->a4_r->setValue(static_cast<int>(ui->increment_4->value()));
    ui->a5_r->setValue(static_cast<int>(ui->increment_5->value()));
    ui->grip_r->setValue(static_cast<int>(ui->increment_6->value()));

    dev.servo_write6(angles, 450);
}

void MainWindow::jog()
{
    switch (following_program)
    {
    case 0:
        connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_stick()));
        time_mod = 300;
        dev.toggleTorque(true);
        if (learning)
            connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command())); // control from the axis is also updated ever 100ms

        break;
    case 1:
        disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(update_stick()));
        time_mod = 1000;
        break;
    }
    following_program = !following_program;
    toggle_jog();
    std::cout << following_program << '\n';
    std::cout.flush();
}
void MainWindow::update_stick()
{

    ui->base_r->setValue(ui->base_r->value() - round(axes[0] * 100) * 0.05);
    ui->a2_r->setValue(ui->a2_r->value() - round(axes[2] * 100) * 0.05);
    ui->a3_r->setValue(ui->a3_r->value() - round(axes[1] * 100) * 0.05);
    ui->a4_r->setValue(ui->a4_r->value() - round(axes[3] * 100) * 0.05);
    ui->a5_r->setValue(ui->a5_r->value() - round(axes[4] * 100) * 0.05);
    ui->grip_r->setValue(ui->grip_r->value() - round(axes[5] * 100) * 0.1);
}

/*void MainWindow::poll_joystick()
{
    while (running)
    {
        // Restrict rate
        usleep(1000);

        // Attempt to sample an event from the joystick
        JoystickEvent event;
        if (joystick->sample(&event))
        {
            if (event.isAxis())
            {
                // std::cout << "Axis " << (int) event.number << " is at " << event.value / 32767.0f<< '\n';
                std::cout.flush();
                axes[event.number] = event.value / 32767.0f;
            }
        }
    }
    return;
}*/

void MainWindow::toggle_jog()
{
    ui->base_r->setDisabled(following_program);
    ui->increment_1->setDisabled(following_program);
    ui->a2_r->setDisabled(following_program);
    ui->increment_2->setDisabled(following_program);
    ui->a3_r->setDisabled(following_program);
    ui->increment_3->setDisabled(following_program);
    ui->a4_r->setDisabled(following_program);
    ui->increment_4->setDisabled(following_program);
    ui->a5_r->setDisabled(following_program);
    ui->increment_5->setDisabled(following_program);
    ui->grip_r->setDisabled(following_program);
    ui->increment_6->setDisabled(following_program);
}

void MainWindow::program()
{
    following_program = !following_program;
    toggle_jog();
    manual_program.clear();
    prog_thread = QtConcurrent::run(this, &MainWindow::RASM_Interpreter, dev.home_position, manual_program);
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

void MainWindow::go_home()
{
    ui->base_r->setValue(dev.home_position[0] - 90);
    ui->a2_r->setValue(dev.home_position[1] - 90);
    ui->a3_r->setValue(dev.home_position[2] - 90);
    ui->a4_r->setValue(dev.home_position[3] - 90);
    ui->a5_r->setValue(dev.home_position[4] - 90);
    ui->grip_r->setValue(dev.home_position[5]);
}