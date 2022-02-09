#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lib/libRobot/include/Arm_lib.hpp"
#include <iostream>
#include <QTimer>
#include <QThread>
#include <cmath>
#include <QtConcurrent>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
using namespace cv;

#define pixMod 45.0847
#define degRad 57.29577951308

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

#define HIDDEN false
#define VISIBLE true;

class WorkerThread : public QThread
{
    Q_OBJECT

    void run()
    {
        QString result;
        uint16_t stepcount = dev->learned_angles.size();
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
        ArmDevice *dev = nullptr;
    signals:
        void resultReady(const QString &s);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        int dir;

        Rect red;
        Rect green;
        Rect blue;
        bool running;

    public slots:
        void toggle_learn_bar();                        // hides or shows the learn bar
        void toggle_camera_bar();                       // hides or shows the camera bar
        void update_axes();                             // prints the axes onto the display
        void command();                                 // issues the commands to the axes

        void learn();                                   // go to learn mode
        void add_step();                                // memorize coordinates for direct learning
        void remove_step();                             // remove coordinate from direct learning
        void follow_path();                                 // execute the memorized coordinates
        void capture();

        void follow();
        //void start_follow();
        void halt();

        void start_follow_red();
        void start_follow_blue();
        void start_follow_green();
        void stop_follow();

        

    private:
        Ui::MainWindow *ui = nullptr;                             // user interface
        bool learn_bar_state = HIDDEN;                  // monitors the state of the bars
        bool camera_bar_state = HIDDEN;
        void set_learn_bar_visibility(bool state);
        void set_camera_bar_visibility(bool state);

        QTimer *Scheduler_100ms = nullptr;                        // timers for different actions
        QTimer *Scheduler_16ms = nullptr;
        QTimer *Scheduler_500ms = nullptr;

        ArmDevice dev;

        friend class WorkerThread;
        WorkerThread learnModeThread;


        QFuture<void> cam_thread;
        QFuture<void> learn_thread;
        

};

#endif // MAINWINDOW_H
