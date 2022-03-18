#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "RobotArm.hpp"
#include <iostream>
#include <QTimer>
#include <QThread>
#include <cmath>
#include <QtConcurrent>
#include <QFileDialog>
#include <QtGamepad/QGamepad>
#include <QTcpSocket>

#include <Instruction.hpp>
#include <isa.hpp>

#ifdef __ARM_NEON
#include <linux/videodev.h>
#else
#include <libv4l1-videodev.h>
typedef float float32_t;
#endif

//#include "lib/libjoystick/joystick.hh"
#include "Instruction.hpp"
#include "isa.hpp"
#include "Variable.hpp"

#include <fstream>


QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

#define HIDDEN false
#define VISIBLE true;

/*class WorkerThread : public QThread       TODO: move to server
{
    Q_OBJECT

    void run()
    {
        QString result;
        //uint16_t stepcount = dev->learned_angles.size();
       // std::cout << stepcount << '\n';
        //std::cout.flush();
        while ( dev -> executing )
        {
            for (int i = 0; i < stepcount && dev -> executing; i++)
            {
                dev->servo_write6(&dev->learned_angles[i][0], 1000);
                usleep(1000000);
            }
        }

        emit resultReady(result);
    }

    public:
        //RobotArm *dev = nullptr;
    signals:
        void resultReady(const QString &s);
};*/            

struct packet
{
    uint8_t command;
    uint16_t data1;
    uint16_t data2;
    uint16_t data3;

};
class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        int dir = 0;
        QString filename;

        void go_home();
        QTimer *Scheduler_100ms = nullptr;                        // timers for different actions
        QTimer *Scheduler_16ms = nullptr;

    public slots:
        void toggle_learn_bar();                        // hides or shows the learn bar
        void toggle_camera_bar();                       // hides or shows the camera bar
        void update_axes();                             // prints the axes onto the display
        void command();                                 // issues the commands to the axes

        void learn();                                   // go to learn mode
        void add_step();                                // memorize coordinates for direct learning
        void remove_step();                             // remove coordinate from direct learning
        void follow_path();                             // execute the memorized coordinates

        //void start_follow();
        void halt();
        void check_if_filedialog();
        void program();
        void update_stick();
        void jog();        

        void start_follow_red();
        void start_follow_green();
        void start_follow_blue();
        void stop_follow();

        void onReadyRead();


    private:
        Ui::MainWindow *ui = nullptr;                             // user interface
        bool learn_bar_state = HIDDEN;                  // monitors the state of the bars
        bool camera_bar_state = HIDDEN;
        void set_learn_bar_visibility(bool state);
        void set_camera_bar_visibility(bool state);
        bool following_program = false;
        bool learning = false;
        bool fileopen = false;

        float32_t axes[6] = { 0 };
        uint16_t time_mod = 1000;


        //RobotArm dev;
        QGamepad* joystick;

        //friend class WorkerThread;
        //WorkerThread learnModeThread;


        QFuture<void> cam_thread;
        QFuture<void> learn_thread;
        QFuture<void> joy_thread;
        QFuture<void> prog_thread;

        void toggle_jog();
        std::vector<int> home_position;
        QTcpSocket sock;


};

#endif // MAINWINDOW_H
