#ifndef ROBOTDASHBOARD_MAINWINDOW_H
#define ROBOTDASHBOARD_MAINWINDOW_H

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
#include "lib/libQuirc/lib/quirc.h"


#include <libv4l1-videodev.h>

#include "Instruction.hpp"
#include "isa.hpp"
#include "Variable.hpp"
#include "BaseTranslationAxis.hpp"

#include <fstream>
#include <cmath>
#include <exception>

using namespace cv;

#define PIX_MOD 45.0847
#define DEG_RAD 57.29577951308

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

#define HIDDEN false
//#define VISIBLE true;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        int dir_ = 0;

        Rect red_;
        Rect green_;
        Rect blue_;
        bool running_;
        QString filename_;
        quirc *decoder_;
        bool hasFinishedRunning_ = true;

        void goHome();
        // uint8_t detectCamera();

        VideoCapture *camera_ = nullptr;

    public slots:
        void toggleLearnBar();                        // hides or shows the learn bar
        void toggleCameraBar();                       // hides or shows the camera bar
        void updateAxes();                             // prints the axes onto the display
        void command();                                 // issues the commands to the axes

        void learn();                                   // go to learn mode
        void addStep();                                // memorize coordinates for direct learning
        void removeStep();                             // remove coordinate from direct learning
        void followLearnedPath();                             // execute the memorized coordinates
        void capture();

        void followColor();

        void startFollowRed();
        void startFollowBlue();
        void startFollowGreen();
        void stopFollow();
        void jog();
        void program();
        void checkIfFiledialog();

        void updateStick();
        //void poll_joystick();
        
        void rasmInterpreter(std::vector <float> homePosition, std::vector<Instruction> program = std::vector<Instruction>());
        void cameraRestarter();
        bool joystickHotplugDetect();
        

    //private:
    public:

        Ui::MainWindow *ui_;                             // user interface

        void setupApplication();
        void initTimers();
        void initButtons();
        void initSliders() const;
        void initCamera();

        bool learnBarState_ = HIDDEN;                  // monitors the state of the bars
        bool cameraBarState_ = HIDDEN;
        void setLearnBarVisibility(bool state);
        void setCameraBarVisibility(bool state);
        bool followingProgram_ = false;
        bool learning_ = false;
        bool fileopen_ = false;

        std::array<float32_t, 6> axes_= {0 };
        uint16_t timeMod_ = 1000;

        QTimer *scheduler100Ms_ = nullptr;                        // timers for different actions
        QTimer *scheduler16Ms_ = nullptr;
        QTimer *scheduler500Ms_ = nullptr;

        RobotArm dev_;
        TranslationAxis base_;
        
        QGamepad* joystick_ = nullptr;

        QFuture<void> camThread_;
        QFuture<void> learnThread_;

        QFuture<void> joyThread_;
        QFuture<void> progThread_;
        std::vector<Instruction> manualProgram_;
        cv::Mat prev_;

        void toggleJog();


};

#endif // ROBOTDASHBOARD_MAINWINDOW_H
