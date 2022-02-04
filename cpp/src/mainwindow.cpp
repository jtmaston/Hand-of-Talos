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
    Scheduler_1s = new QTimer(this);

    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_axes())); // axis readout is updated every 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));     // control from the axis is also updated ever 100ms
    connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));      // camera is updated every 20ms

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));  // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar())); // when the track button is clicked, toggle the bar

    connect(ui->next, SIGNAL(clicked()), SLOT(add_step()));    //  <<
    connect(ui->prev, SIGNAL(clicked()), SLOT(remove_step())); //  buttons for the learn mode
    connect(ui->execute, SIGNAL(clicked()), SLOT(follow_path()));  //  >>

    connect(ui->follow_red, SIGNAL(clicked()), SLOT(start_follow_red()));
    connect(ui->follow_green, SIGNAL(clicked()), SLOT(start_follow_green()));
    connect(ui->follow_blue, SIGNAL(clicked()), SLOT(start_follow_blue()));

    connect(ui->halt_btn, SIGNAL(clicked()), SLOT(halt()));

    Scheduler_100ms->start(100);
    Scheduler_16ms->start(16);
    Scheduler_1s->start(500);

    dev.home_position(); // reset the robot to the home position

    camera = new VideoCapture(0, CAP_V4L);

    camera->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera->set(cv::CAP_PROP_FPS, 60);
    camera->set(cv::CAP_PROP_BRIGHTNESS, 25);

    ui->a4_r->setValue(-90);

    // robot.reset();
}

MainWindow::~MainWindow()
{
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
    // std::cout << angles[0] << '\n';

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
    float32_t *angle = dev.servo_readall(); // read all the servo values
    std::vector<float32_t> t(6);            // make them into a vector
    memmove(&t[0], &angle[0], 6 * sizeof(float32_t));
    dev.learned_angles.push_back(std::move(t)); // and add it onto the command queue
    delete angle;

    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str())); // then update the label
}

void MainWindow::remove_step() // remove a step from the queue
{
    dev.learned_angles.pop_back();
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str())); // then update the label
}

void MainWindow::follow_path() // start to run
{   
    dev.toggleTorque(true);
    std::cout << dev.executing;
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
    *camera >> frame;

    Mat imgHSV;
    //Mat clear = frame.clone();
    //cv::medianBlur(frame, frame, 5);


    cvtColor(frame, imgHSV, COLOR_BGR2HSV); // convert the image to HSV, or Hue Saturation Value

    std::vector<uint16_t> red_lower = {0, 50, 50}; // set the tresholds for colors, red green and blue
    std::vector<uint16_t> red_upper = {10, 255, 255};
    std::vector<uint16_t> red_lower_mask1 = {170, 50, 50};
    std::vector<uint16_t> red_upper_mask1 = {180, 255, 255};

    Point center_line_top;
    Point center_line_btm;

    center_line_top.x = 320;
    center_line_top.y = 0;

    center_line_btm.x = 320;
    center_line_btm.y = 480;

    line(frame, center_line_top, center_line_btm, CV_RGB(255, 0, 0), 1);

    center_line_top.x = 0;
    center_line_top.y = 240;

    center_line_btm.x = 640;
    center_line_btm.y = 240;

    line(frame, center_line_top, center_line_btm, CV_RGB(255, 0, 0), 1);

    Mat inverted = ~frame;
    cvtColor(inverted, inverted, COLOR_BGR2HSV);

    Mat imgTreshRed;
    Mat imgTreshRed1;
    inRange(imgHSV, red_lower, red_upper, imgTreshRed);
    inRange(imgHSV, red_lower_mask1, red_upper_mask1, imgTreshRed1);

    imgTreshRed += imgTreshRed1;
    // cvtColor();

    // inRange(frame, Scalar(90 - 10, 70, 50), Scalar(90 + 10, 255, 255), imgTreshRed);

    std::vector<uint16_t> green_lower = {45, 72, 92};
    std::vector<uint16_t> green_upper = {102, 255, 255};

    Mat imgTreshGreen;
    inRange(imgHSV, green_lower, green_upper, imgTreshGreen);

    std::vector<uint16_t> blue_lower = {112, 60, 63};
    std::vector<uint16_t> blue_upper = {124, 255, 255};

    Mat imgTreshBlue;
    inRange(imgHSV, blue_lower, blue_upper, imgTreshBlue);

    Mat kernel;
    kernel.ones(5, 5, CV_8U);

    dilate(imgTreshRed, imgTreshRed, kernel);
    Mat res_red;
    bitwise_and(frame, frame, res_red, imgTreshRed);

    dilate(imgTreshGreen, imgTreshGreen, kernel);
    Mat res_green;
    bitwise_and(frame, frame, res_green, imgTreshGreen);

    dilate(imgTreshBlue, imgTreshBlue, kernel);
    Mat res_blue;
    bitwise_and(frame, frame, res_blue, imgTreshBlue);

    std::vector<std::vector<Point>> contours;
    findContours(imgTreshRed, contours, RETR_TREE, CHAIN_APPROX_TC89_KCOS);
    int num = contours.size();

    int max = 0;
    int ind = 0;

    for (int i = 0; i < num; i++)
        if (contourArea(contours[i]) > max)
        {
            ind = i;
            max = contourArea(contours[i]);
        }

    // for( int i = 0 ; i < num; i++)
    //{
    // if ( contourArea(contours[i]) > 0)
    //{
    if (max > 0)
    {
        red = boundingRect(contours[ind]);
        Point pt1, pt2;
        pt1.x = red.x;
        pt1.y = red.y;
        pt2.x = red.x + red.width;
        pt2.y = red.y + red.height;
        // Draws the rect in the original image and show it
        rectangle(frame, pt1, pt2, CV_RGB(255, 0, 0), 3);
        putText(frame, "Red", pt1, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255));
    }
    //}

    //}
    contours.clear();
    findContours(imgTreshGreen, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    num = contours.size();
    max = 0;
    for (int i = 0; i < num; i++)
        if (contourArea(contours[i]) > max)
        {
            ind = i;
            max = contourArea(contours[i]);
        }

    // for( int i = 0 ; i < num; i++)
    //{
    // if ( contourArea(contours[i]) > 300)
    //{
    if (max > 0)
    {
        green = boundingRect(contours[ind]);
        Point pt1, pt2;
        pt1.x = green.x;
        pt1.y = green.y;
        pt2.x = green.x + green.width;
        pt2.y = green.y + green.height;
        // Draws the rect in the original image and show it
        rectangle(frame, pt1, pt2, CV_RGB(0, 255, 0), 3);
        putText(frame, "Green", pt1, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0));
    }
    //}

    //}
    contours.clear();
    findContours(imgTreshBlue, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    num = contours.size();

    max = 0;
    for (int i = 0; i < num; i++)
        if (contourArea(contours[i]) > max)
        {
            ind = i;
            max = contourArea(contours[i]);
        }

    if (max > 0)
    {
        blue = boundingRect(contours[ind]);
        Point pt1, pt2;
        pt1.x = blue.x;
        pt1.y = blue.y;
        pt2.x = blue.x + blue.width;
        pt2.y = blue.y + blue.height;
        // Draws the rect in the original image and show it

        rectangle(frame, pt1, pt2, CV_RGB(0, 0, 255), 3);
        putText(frame, "Blue", pt1, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0));

        pt1.x = blue.x + blue.width / 2;
        pt2.x = blue.x + blue.width / 2;
        line(frame, pt1, pt2, CV_RGB(0, 0, 255), 1);
    }
    //}

    //}

    // cvtColor(frame, frame,  COLOR_HSV2BGR);
    //frame = clear;
    qt_image = QImage((const unsigned char *)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));

    // ui->viewfinder->resize(1260);
}

void MainWindow::halt()
{
    dev.toggleTorque(false);
}

void MainWindow::start_follow_red()
{
    dir = 1;
    connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
}

void MainWindow::start_follow_green()
{
    dir = 2;
    connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
}

void MainWindow::start_follow_blue()
{
    dir = 3;
    connect(Scheduler_1s, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
    //follow();
    /*
    dir = 3;
    float32_t lCam = 4.8 * pixMod;

    int Ox_deviation = abs(320 - blue.x);

    std::cout << lCam << " " << Ox_deviation << '\n';

    float32_t angle = acos( Ox_deviation / lCam ) * (180.0/3.141592653589793238463) ;
    std::cout << angle << '\n';*/

    // x_center = red.x + red.width / 2;
    // y_center = red.y + red.y / 2;
    /*connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(follow()));
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));*/

    /*float32_t lCam = 4.8 * pixMod;
    int deviation = blue.x - 320;
    float32_t alpha = asin( deviation / lCam ) * degRad;
    std::cout << alpha;

    std::cout << "here";
    std::cout.flush();
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
    const float64_t modifier = 110.0 / 640.0;
    // int deviation = int(floor(dev.servo_read(1))) + (((blue.x + ( blue.width / 2.0 )) - 320) * modifier);

    dev.toggleTorque(true);
    dev.servo_write(1, 90 + 55, 3000);*/
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

    /*if (x_center < 300)
    {
        ui->base_r->setValue(ui->base_r->value() + 1);
    }
    else if (x_center > 340)
    {
        ui->base_r->setValue(ui->base_r->value() - 1);
    }

    if (y_center < 210)
    {
        ui->a4_r->setValue(ui->a4_r->value() + 1);
    }
    else if (y_center > 270)
    {
        ui->a4_r->setValue(ui->a4_r->value() - 1);
    }*/
    

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
