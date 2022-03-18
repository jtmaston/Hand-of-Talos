#include <server.hpp>
#include <iostream>
#include <QThread>
#include <QDataStream>
#include <sstream>
#include <cstring>


#define RED 31
#define GRN 32
#define BLU 33


Server::Server(QObject* parent) :
    QObject(parent)
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer, SIGNAL(newConnection()),
            this, SLOT(acceptConnection())
    );

    if(! tcpServer -> listen(QHostAddress::Any, 8123))
        std::cout << "[ERR]: Bind failed!\n";
    else
        std::cout << "[INFO]: Server listening\n";

    camera = new VideoCapture(0, CAP_V4L2);
    camera->set(cv::CAP_PROP_FRAME_WIDTH, 640);
    camera->set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    camera->set(cv::CAP_PROP_FPS, 60);

    Scheduler_100ms = new QTimer(this); // timer called every 100ms
    Scheduler_16ms = new QTimer(this);  // timer called every  20ms
    Scheduler_500ms = new QTimer(this);

    connect(Scheduler_16ms, SIGNAL(timeout()), SLOT(capture()));

    Scheduler_100ms->start(100);
    Scheduler_16ms->start(16);
    Scheduler_500ms->start(500);

    dev.toggleTorque(true);
}

Server::~Server()
{
    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
}

void Server::acceptConnection()
{
    clientSock = tcpServer->nextPendingConnection();
    connect(clientSock, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(clientSock, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
    clientSock->write(QByteArray::fromStdString(clientSock->peerAddress().toString().toStdString() + " connected to server !\n"));
    std::cout << "[INFO]: Client " << clientSock->peerAddress().toString().toStdString() << " connected!";
    std::cout.flush();
}

QDataStream &operator<<(QDataStream &out, const Instruction &item)
{  
    out << item.opcode;
    for(int i = 0 ; i < 10; i++)
        out << item.params[i];
    return out;
}
QDataStream &operator>>(QDataStream &in, Instruction &item)
{
    in >> item.opcode;
    for(int i = 0 ; i < 10; i++)
        in >> item.params[i];
    return in;
}

void Server::onReadyRead()
{
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    //QByteArray datas = sender->read(sizeof(Instruction));
    QByteArray datas = sender->readLine();

    std::cout << datas.size() << '\n';

    Instruction instruction;
    memcpy(&instruction, datas.data(), sizeof(Instruction));
    // hm.

    switch (instruction.opcode)
    {

        case ANG:
            dev.servo_write(instruction.params[0], instruction.params[1], time_mod);
            std::cout << "angle\n";
            break;

        case ANGS:
        {
            uint16_t angles[] = { instruction.params[0], instruction.params[1], instruction.params[2],
                                  instruction.params[3], instruction.params[4], instruction.params[5]
                                   };
            dev.servo_write6(angles, time_mod);

            std::cout << "Got 6 ang request: ";
            for(int i = 0 ; i < 6; i++)
                std::cout << angles[i] << " ";
            std::cout << '\n';

            break;
        }

        case SPD:
            time_mod = instruction.params[0];
            break;

        case GHME:
            dev.go_home();
            break;

        case SHME: // TODO
            break;
        
        case RED:
            break;
    }


    return;

}

void Server::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{   
    if (socketState == QAbstractSocket::UnconnectedState)
    {
        std::cout << "[INFO]: Client " << clientSock->peerAddress().toString().toStdString() << " disconnected!";
        clientSock->close();
        clientSock = nullptr;
    }
}

void Server::start_follow_red()
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

void Server::start_follow_green()
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

void Server::start_follow_blue()
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

void Server::stop_follow()
{
    disconnect(Scheduler_500ms, SIGNAL(timeout()), this, SLOT(follow()));
    connect(Scheduler_100ms, SIGNAL(timeout()), SLOT(command()));
    uint8_t cmd[] = {0x07, 0};
    write(dev.led_bus, cmd, 2);
}

void Server::follow()
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

    // TODO: SEND OVER THE WIRE
    //ui->base_r->setValue(ui->base_r->value() + round(alpha));
    //ui->a4_r->setValue(ui->a4_r->value() + round(beta));

    /*uint16_t angles[6];
    angles[0] = ui->base_r->value() + 90; // need to adjust with 90
    angles[1] = ui->a2_r->value() + 90;
    angles[2] = ui->a3_r->value() + 90;
    angles[3] = ui->a4_r->value() + 90;
    angles[4] = ui->a5_r->value() + 90;
    angles[5] = ui->grip_r->value() + 90;*/
    // TODO: need to actually set the angles

    //dev.servo_write6(angles, 450);
}

void Server::capture() // this is 2am code.
{                          // runs the viewfinder, alongside color detection

    //detect_camera();
    int connected = true;
    bool cap = 0;

    Mat frame;
    // QImage qt_image;

    //while (this->running)
    //{
        //auto start = std::chrono::high_resolution_clock::now();
        //*camera >> frame; // note: this line is blocking, restricting the loop to run at most at the speed of the camera
        connected = camera->read(frame);
        //std::cout << connected << '\n';
        //if (!connected)
          //  break;

        Mat imgHSV;

        cvtColor(frame, imgHSV, COLOR_BGR2HSV);                            // convert the image to HSV, or Hue Saturation Value
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


        // TODO: send via rtsp
        //ui->viewfinder->setPixmap(QPixmap::fromImage(qt_image.rgbSwapped()));
        //ui->viewfinder->updateGeometry();

        //auto end = std::chrono::high_resolution_clock::now();
        //double delta = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        //if( delta < 33.3){

        //    std::this_thread::sleep_for(std::chrono::microseconds(int(floor(delta * 1000))));
        //}
    //}

    //camera->release();

    //std::cout << "Exited";
    //std::cout.flush();
    return;
}

void Server::learn() // starts the learn mode
{
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command())); // stop the control function
    dev.go_home();                                                             // move back to home
    dev.toggleTorque(false);                                               // and disable the torque
    learning = true;
}

void Server::add_step() // add a step
{
    float32_t *angle = dev.servo_readall(); // read all the servo values
    std::vector<float32_t> t(6);            // make them into a vector
    memmove(&t[0], &angle[0], 6 * sizeof(float32_t));
    dev.learned_angles.push_back(std::move(t)); // and add it onto the command queue
    delete angle;

    
}

void Server::remove_step() // remove a step from the queue
{
    dev.learned_angles.pop_back();
    
}

void Server::follow_path() // start running
{
    disconnect(Scheduler_100ms, SIGNAL(timeout()), this, SLOT(command()));
    dev.toggleTorque(true);
    dev.executing = !dev.executing; // set executing in order to stop

    time_mod = 1000;

    if (dev.executing)
    {
        dev.toggleTorque(true);
        //learnModeThread.dev = &dev;
        //learnModeThread.start();
        // TODO: implement threadless
    }
    std::cout.flush();
}

void Server::jog()
{
    // TODO: IMPLEMENT ON OTHER END, CALL OVER WIRE
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
    
    std::cout << following_program << '\n';
    std::cout.flush();
}

