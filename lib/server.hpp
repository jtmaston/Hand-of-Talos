#pragma once
#include <Arm_lib.hpp>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <string>
#include <RobotArm.hpp>
#include <Instruction.hpp>
#include <isa.hpp>
#include <QMainWindow>
#include <iostream>
#include <QTimer>
#include <QThread>
#include <cmath>
#include <QtConcurrent>
#include <QFileDialog>
#include <QtGamepad/QGamepad>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <Instruction.hpp>
#include <isa.hpp>

#ifdef __ARM_NEON
#include <linux/videodev.h>
#else
#include <libv4l1-videodev.h>
typedef float float32_t;
#endif

using namespace cv;
#define pixMod 45.0847
#define degRad 57.29577951308

class Server : public QObject
{
    Q_OBJECT
    public:
        explicit Server(QObject* parent = nullptr);
        virtual ~Server();

        Rect red;
        Rect green;
        Rect blue;
        bool running;
        VideoCapture *camera = nullptr;

        QTimer *Scheduler_100ms = nullptr;                        // timers for different actions
        QTimer *Scheduler_16ms = nullptr;
        QTimer *Scheduler_500ms = nullptr;


    private:
        int dir = 0;
        bool learning;
        bool following_program;

        void initServer();
        QTcpServer* tcpServer = nullptr;
        QTcpSocket* clientSock = nullptr;
        RobotArm dev;
        uint16_t time_mod;
    
    signals:

    public slots:
        void acceptConnection();
        void onSocketStateChanged(QAbstractSocket::SocketState socketState);
        void onReadyRead();
        void start_follow_red();
        void start_follow_blue();
        void start_follow_green();
        void stop_follow();
        void jog();
        void learn();                                   // go to learn mode
        void add_step();                                // memorize coordinates for direct learning
        void remove_step();                             // remove coordinate from direct learning
        void follow_path();                             // execute the memorized coordinates
        void capture();
        void follow();
        void RASM_Interpreter();
};