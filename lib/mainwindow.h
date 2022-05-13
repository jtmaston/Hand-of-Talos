#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RobotArm.hpp"
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



#ifdef __ARM_NEON
#include <linux/videodev.h>
#else
#include <libv4l1-videodev.h>
#endif

//#include "lib/libjoystick/joystick.hh"
#include "Instruction.hpp"
#include "isa.hpp"
#include "Variable.hpp"
#include "BaseTranslationAxis.hpp"

#include <fstream>


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

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        int dir = 0;

        Rect red;
        Rect green;
        Rect blue;
        bool running;
        QString filename;

        void go_home();
        uint8_t detect_camera();
        VideoCapture *camera = nullptr;
        cv::ErrorCallback camera_error_handler(int status, const char* func_name, const char* err_msg, 
                   const char* file_name, int line, void*);

    public slots:
        void toggle_learn_bar();                        // hides or shows the learn bar
        void toggle_camera_bar();                       // hides or shows the camera bar
        void update_axes();                             // prints the axes onto the display
        void command();                                 // issues the commands to the axes

        void learn();                                   // go to learn mode
        void add_step();                                // memorize coordinates for direct learning
        void remove_step();                             // remove coordinate from direct learning
        void follow_path();                             // execute the memorized coordinates
        void capture();

        void follow();
        void halt();

        void start_follow_red();
        void start_follow_blue();
        void start_follow_green();
        void stop_follow();
        void jog();
        void program();
        void check_if_filedialog();

        void update_stick();
        //void poll_joystick();
        
        void RASM_Interpreter(const std::vector <float> home_position, const std::vector<Instruction> program = std::vector<Instruction>());
        void camera_restarter();
        

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

        QTimer *Scheduler_100ms = nullptr;                        // timers for different actions
        QTimer *Scheduler_16ms = nullptr;
        QTimer *Scheduler_500ms = nullptr;

        RobotArm dev;
        BaseTranslationAxis base;
        
        QGamepad* joystick;


        QFuture<void> cam_thread;
        QFuture<void> learn_thread;
        QFuture<void> joy_thread;
        QFuture<void> prog_thread;
        std::vector<Instruction> manual_program;
        cv::Mat prev;

        void toggle_jog();


};

#endif // MAINWINDOW_H
