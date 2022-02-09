#include "../lib/mainwindow.h"
#include "../lib/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);

    Scheduler_100ms = new QTimer(this); // timer called every 100ms
    Scheduler_16ms = new QTimer(this);  // timer called every  20ms
    Scheduler_500ms = new QTimer(this);

    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_axes())); // axis readout is updated every 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));     // control from the axis is also updated ever 100ms
    //connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));      // camera is updated every 20ms
    

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));  // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar())); // when the track button is clicked, toggle the bar

    connect(ui->next, SIGNAL(clicked()), SLOT(add_step()));               //  <<
    connect(ui->prev, SIGNAL(clicked()), SLOT(remove_step()));            //  buttons for the learn mode
    connect(ui->execute, SIGNAL(clicked()), SLOT(follow_path()));         //  >>
    connect(ui->follow_stop, SIGNAL(clicked()), SLOT(stop_follow()));         //  >>

    connect(ui->follow_red, SIGNAL(clicked()), SLOT(start_follow_red()));
    connect(ui->follow_green, SIGNAL(clicked()), SLOT(start_follow_green()));
    connect(ui->follow_blue, SIGNAL(clicked()), SLOT(start_follow_blue()));

    connect(ui->halt_btn, SIGNAL(clicked()), SLOT(halt()));

    Scheduler_100ms->start(100);
    Scheduler_16ms->start(16);
    Scheduler_500ms->start(500);

    dev.home_position(); // reset the robot to the home position

    

    ui->a4_r->setValue(-90);
    running = true;
    cam_thread = QtConcurrent::run(this, &MainWindow::capture);
}

MainWindow::~MainWindow()
{
    running = false;
    cam_thread.cancel();
    delete ui;
}
void MainWindow::toggle_learn_bar()            // toggle the learning bar,
{
    set_camera_bar_visibility(HIDDEN);         // hide the camera bar
    learn_bar_state = !learn_bar_state;        // << set if the learning bar is visible or not
    set_learn_bar_visibility(learn_bar_state); // >>
    learn();                                   // and go into the learn mode
}

void MainWindow::toggle_camera_bar()           // ditto for the camera bar
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

    ui->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(data[0])).c_str()); // and set the strings for
    ui->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(data[1])).c_str()); // the labels
    ui->a3_d->setText(std::string(std::string("Axis 3: ") + std::to_string(data[2])).c_str());
    ui->a4_d->setText(std::string(std::string("Axis 4: ") + std::to_string(data[3])).c_str());
    ui->a5_d->setText(std::string(std::string("Axis 5: ") + std::to_string(data[4])).c_str());
    ui->a6_d->setText(std::string(std::string("Axis 6: ") + std::to_string(data[5])).c_str());
    delete data; // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}

void MainWindow::command() // get the values from the sliders, then write them on the bus
{
    uint16_t angles[6];
    angles[0] = ui->base_r->value() + 90; // need to adjust with 90
    angles[1] = ui->a2_r->value() + 90;
    angles[2] = ui->a3_r->value() + 90;
    angles[3] = ui->a4_r->value() + 90;
    angles[4] = ui->a5_r->value() + 90;
    angles[5] = ui->grip_r->value() + 90;

    dev.servo_write6(angles, 1000); // move the axes
}

void MainWindow::learn() // starts the learn mode
{
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command())); // stop the control function
    dev.home_position();                                                   // move back to home
    dev.toggleTorque(false);                                               // and disable the torque
}

void MainWindow::add_step() // add a step
{
    float32_t *angle = dev.servo_readall();                                 // read all the servo values
    std::vector<float32_t> t(6);                                            // make them into a vector
    memmove(&t[0], &angle[0], 6 * sizeof(float32_t));
    dev.learned_angles.push_back(std::move(t));                             // and add it onto the command queue
    delete angle;

    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str())); // then update the label
}

void MainWindow::remove_step()      // remove a step from the queue
{
    dev.learned_angles.pop_back();
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str())); // then update the label
}

void MainWindow::follow_path()      // start running
{   
    dev.toggleTorque(true);
    dev.executing = !dev.executing; // set executing in order to stop

    if (dev.executing)
    {
        dev.toggleTorque(true);
        learnModeThread.dev = &dev;
        learnModeThread.start();
    }
    std::cout.flush();
}

void MainWindow::capture() // this is 2am code.
{                          // runs the viewfinder, alongside color detection

    Mat frame;
    QImage qt_image;
    VideoCapture* camera = nullptr;

    camera = new VideoCapture(0, CAP_V4L);

    camera->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera->set(cv::CAP_PROP_FPS, 60);
    camera->set(cv::CAP_PROP_BRIGHTNESS, 25);

    
    while( running )
    {
        try{
            *camera >> frame;

            Mat imgHSV;

            cvtColor(frame, imgHSV, COLOR_BGR2HSV); // convert the image to HSV, or Hue Saturation Value
            line(frame, Point(320, 0), Point(320, 480), CV_RGB(255, 0, 0), 1);  // draw the crosshair
            line(frame, Point(0, 240), Point(640, 240), CV_RGB(255, 0, 0), 1);
                                                                                // to hue saturation values, for easier processing
            Mat imgTreshRed;
            Mat imgTreshRed1;
            inRange(imgHSV, Scalar(0, 50, 50), Scalar(10, 255, 255), imgTreshRed);     // we treshold the image, removing every color but red
            inRange(imgHSV, Scalar(170, 50, 50), Scalar(180, 255, 255), imgTreshRed1);
            imgTreshRed += imgTreshRed1;

            Mat imgTreshGreen;
            inRange(imgHSV, Scalar(45, 72, 92), Scalar(102, 255, 255), imgTreshGreen);

            Mat imgTreshBlue;
            inRange(imgHSV, Scalar(112, 60, 63), Scalar(124, 255, 255), imgTreshBlue);

            Mat res_red;                                                        // by doing bitwise and with the treshold. anything that isn't
            bitwise_and(frame, frame, res_red, imgTreshRed);                    // red, green or blue automatically gets turned to 0 ( as a pixel )
                                                                                // with bitwise and, they get destroyed
            Mat res_green;
            bitwise_and(frame, frame, res_green, imgTreshGreen);

            Mat res_blue;
            bitwise_and(frame, frame, res_blue, imgTreshBlue);

            std::vector<std::vector<Point>> contours;
            findContours(imgTreshRed, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

            int max = 0;
            int ind = 0;

            for (auto& contour: contours)
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
            for (auto& contour: contours)
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
            for (auto& contour: contours)
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
            qt_image = QImage((const unsigned char *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
            qt_image = qt_image.scaled(751, 481);
            ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
        }catch(const std::exception& ex)
        {
            
        }
    }
}

void MainWindow::halt()
{
    dev.toggleTorque(false);
}

void MainWindow::start_follow_red()
{
    dir = 1;
    connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
}

void MainWindow::start_follow_green()
{
    dir = 2;
    connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
}

void MainWindow::start_follow_blue()
{
    dir = 3;
    connect(Scheduler_500ms, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
}

void MainWindow::stop_follow()
{
    disconnect(Scheduler_500ms, SIGNAL(timeout()), this, SLOT(follow()));
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));
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
    float32_t alpha = ( asin( ( 320 - x_center ) / ((22) * pixMod) ) * 180 ) / __PI__;
    float32_t beta = ( asin( ( 240 - y_center ) / ((36) * pixMod) ) * 180 ) / __PI__;

    ui->base_r->setValue(ui->base_r->value() + round(alpha));
    ui->a4_r->setValue(ui->a4_r->value() + round(beta));

    uint16_t angles[6];
    angles[0] = ui->base_r->value() + 90; // need to adjust with 90
    angles[1] = ui->a2_r->value() + 90;
    angles[2] = ui->a3_r->value() + 90;
    angles[3] = ui->a4_r->value() + 90;
    angles[4] = ui->a5_r->value() + 90;
    angles[5] = ui->grip_r->value() + 90;

    dev.servo_write6(angles, 450);
}
