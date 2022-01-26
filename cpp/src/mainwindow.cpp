#include "../lib/mainwindow.h"
#include "../lib/ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    set_camera_bar_visibility(HIDDEN);
    set_learn_bar_visibility(HIDDEN);

    Scheduler_100ms = new QTimer(this);                                         // timer called every 100ms
    Scheduler_16ms  = new QTimer(this);                                         // timer called every  20ms

    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(update_axes()));           // axis readout is updated every 100ms
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));               // control from the axis is also updated ever 100ms
    connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));                // camera is updated every 20ms

    connect(ui->learn_btn, SIGNAL(clicked()), SLOT(toggle_learn_bar()));        // when the learn button is clicked, toggle the bar
    connect(ui->track_btn, SIGNAL(clicked()), SLOT(toggle_camera_bar()));       // when the track button is clicked, toggle the bar

    connect(ui->next, SIGNAL(clicked()), SLOT(add_step()));                     //  <<
    connect(ui->prev, SIGNAL(clicked()), SLOT(remove_step()));                  //  buttons for the learn mode
    connect(ui->execute, SIGNAL(clicked()), SLOT(execute()));                   //  >>
    

    Scheduler_100ms -> start(100);
    Scheduler_16ms -> start(16);

    dev.home_position();                                                        // reset the robot to the home position
    
    camera = new VideoCapture(0, CAP_V4L);

    camera -> set(cv::CAP_PROP_FRAME_WIDTH, 640); // valueX = your wanted width
    camera -> set(cv::CAP_PROP_FRAME_HEIGHT, 480); // valueY = your wanted heigth
    camera -> set(cv::CAP_PROP_FPS, 60); // valueY = your wanted heigth

    ui -> a4_r -> setValue(-90);

    // robot.reset();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::toggle_learn_bar()                     // toggle the learning bar, 
{
    set_camera_bar_visibility(HIDDEN);                  // hide the camera bar
    learn_bar_state = !learn_bar_state;                 // << set if the learning bar is visible or not
    set_learn_bar_visibility(learn_bar_state);          // >>
    learn();                                            // and go into the learn mode
}

void MainWindow::toggle_camera_bar()                    // ditto for the camera bar
{       
    set_learn_bar_visibility(HIDDEN);
    camera_bar_state = !camera_bar_state;
    set_camera_bar_visibility(camera_bar_state);
}

void MainWindow::set_camera_bar_visibility(bool state)  // hide the camera bar, by setting height to 0
{
    camera_bar_state = state;
    ui->follow_blue->setVisible(camera_bar_state);
    ui->follow_red->setVisible(camera_bar_state);
    ui->follow_green->setVisible(camera_bar_state);
    ui->follow_stop->setVisible(camera_bar_state);
}

void MainWindow::set_learn_bar_visibility(bool state)   // hide the learn bar, by setting the height to 0
{
    learn_bar_state = state;
    ui->execute->setVisible(learn_bar_state);
    ui->next->setVisible(learn_bar_state);
    ui->prev->setVisible(learn_bar_state);
}

void MainWindow::update_axes()                          // this updates the axes display
{
    float32_t *data = dev.servo_readall();              // read the values from all of the servos

    ui->a1_d->setText(std::string(std::string("Axis 1: ") + std::to_string(data[0])).c_str());      // and set the strings for
    ui->a2_d->setText(std::string(std::string("Axis 2: ") + std::to_string(data[1])).c_str());      // the labels
    ui->a3_d->setText(std::string(std::string("Axis 3: ") + std::to_string(data[2])).c_str());
    ui->a4_d->setText(std::string(std::string("Axis 4: ") + std::to_string(data[3])).c_str());
    ui->a5_d->setText(std::string(std::string("Axis 5: ") + std::to_string(data[4])).c_str());
    ui->a6_d->setText(std::string(std::string("Axis 6: ") + std::to_string(data[5])).c_str());
    delete data;                                        // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}

void MainWindow::command()                              // get the values from the sliders, then write them on the bus
{
    uint16_t angles[6];
    angles[0] = ui->base_r->value() + 90;               // need to adjust with 90
    angles[1] = ui->a2_r->value() + 90;
    angles[2] = ui->a3_r->value() + 90;
    angles[3] = ui->a4_r->value() + 90;
    angles[4] = ui->a5_r->value() + 90;
    angles[5] = ui->grip_r->value() + 90;

    dev.servo_write6(angles, 1000);                     // move the axes
}

void MainWindow::learn()                                // starts the learn mode
{
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));      // stop the control function
    dev.home_position();                                                        // move back to home
    dev.toggleTorque(false);                                                    // and disable the torque
}

void MainWindow::add_step()                             // add a step
{
    float32_t *angle = dev.servo_readall();             // read all the servo values
    std::vector<float32_t> t(6);                        // make them into a vector
    memmove(&t[0], &angle[0], 6 * sizeof(float32_t));
    dev.learned_angles.push_back(std::move(t));         // and add it onto the command queue
    delete angle;  
    
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() + 1).c_str()));     // then update the label
}

void MainWindow::remove_step()                          // remove a step from the queue
{
    dev.learned_angles.pop_back();
    ui->execute->setText(QString(std::to_string(ui->execute->text().toInt() - 1).c_str()));     // then update the label
}

void MainWindow::execute()                              // start to run
{
    dev.executing = !dev.executing;                     // set executing in order to stop

    switch (dev.executing)
    {
    case true:
    {
        learnModeThread.dev = &dev;
        learnModeThread.start();

        dev.toggleTorque(true);                         // enable the torque 
        break;
    }
    }
}

void MainWindow::capture()
{
    *camera >> frame;
    
    Mat imgHSV;
    cvtColor(frame, imgHSV,  COLOR_BGR2HSV);

    std::vector<uint16_t> red_lower = {170, 70, 50};
    std::vector<uint16_t> red_upper = {180, 255, 255};

    Mat imgTreshRed;
    inRange(imgHSV, red_lower, red_upper, imgTreshRed);

    std::vector<uint16_t> green_lower = {25, 52, 72};
    std::vector<uint16_t> green_upper = {102, 255, 255};

    Mat imgTreshGreen;
    inRange(imgHSV, green_lower, green_upper, imgTreshGreen);

    std::vector<uint16_t> blue_lower = {94, 80, 2};
    std::vector<uint16_t> blue_upper = {120, 255, 255};

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

    std::vector<std::vector<Point> > contours;
    findContours(imgTreshRed, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    int num = contours.size();
    for( int i = 0 ; i < num; i++)
    {
        if ( contourArea(contours[i]) > 0)
        {
            Rect rect = boundingRect( contours[i] );
            Point pt1, pt2;
            pt1.x = rect.x;
            pt1.y = rect.y;
            pt2.x = rect.x + rect.width;
            pt2.y = rect.y + rect.height;
            // Draws the rect in the original image and show it
            rectangle(frame, pt1, pt2, CV_RGB(255,0,0), 3);
            putText(frame, "Red", pt1, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 0, 255));
        }
        
    }
    contours.clear();
    findContours(imgTreshGreen, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    num = contours.size();
    for( int i = 0 ; i < num; i++)
    {
        if ( contourArea(contours[i]) > 300)
        {
            Rect rect = boundingRect( contours[i] );
            Point pt1, pt2;
            pt1.x = rect.x;
            pt1.y = rect.y;
            pt2.x = rect.x + rect.width;
            pt2.y = rect.y + rect.height;
            // Draws the rect in the original image and show it
            rectangle(frame, pt1, pt2, CV_RGB(0,255,0), 1);
            putText(frame, "Green", pt1, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0));
        }
        
    }
    contours.clear();
    findContours(imgTreshBlue, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    num = contours.size();
    for( int i = 0 ; i < num; i++)
    {
        if ( contourArea(contours[i]) > 300)
        {
            Rect rect = boundingRect( contours[i] );
            Point pt1, pt2;
            pt1.x = rect.x;
            pt1.y = rect.y;
            pt2.x = rect.x + rect.width;
            pt2.y = rect.y + rect.height;
            // Draws the rect in the original image and show it
            rectangle(frame, pt1, pt2, CV_RGB(0,0,255), 1);
            putText(frame, "Blue", pt1, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0));
        }
        
    }

    //cvtColor(frame, frame,  COLOR_HSV2BGR);
    qt_image = QImage((const unsigned char*) (frame.data), frame.cols, frame.rows, QImage::Format_BGR888);
    ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image));

    //ui->viewfinder->resize(1260);
}