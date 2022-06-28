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
#include <QMessageBox>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <quirc.h>

#include <libv4l1-videodev.h>

#include "Instruction.hpp"
#include "isa.hpp"
#include "Variable.hpp"
#include "BaseTranslationAxis.hpp"
#include "Image.hpp"

#include <fstream>
#include <exception>

#include <mutex>

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
        struct quirc *decoder;

        void go_home();
        uint8_t detect_camera();
        VideoCapture *camera = nullptr;
        
        std::vector<Instruction>* instruction_queue;
        std::vector<Instruction>* interrupt_vector;
        bool interrupt = false, active = true, nodelay = false;

        void init_peripherals();
        void init_signals();


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
        
        void RASM_Interpreter(const std::vector <float>, std::vector<Instruction>*, std::vector<Instruction>*, bool*, bool*);
        void camera_restarter();
        bool joystick_hotplug_detect();
        void update_viewfinder();
        void postprocess();
        

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
        
        QGamepad* joystick = nullptr;
        QGamepadManager* gamepad_manager;


        QFuture<void> cam_thread;
        QFuture<void> process_thread;

        QFuture<void> learn_thread;
        QFuture<void> prog_thread;
        std::vector<Instruction> manual_program;
        cv::Mat frame;

        void toggle_jog();
        std::mutex anti_freewheel;
        std::mutex synchroMesh;
        std::mutex displaySync;


};

#endif // MAINWINDOW_H
