#ifndef ROBOTDASHBOARD_MAINWINDOW_HPP
#define ROBOTDASHBOARD_MAINWINDOW_HPP

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

#ifndef __x86_64
#include <libv4l1-videodev.h>
#else
#include <linux/videodev2.h>
#endif


#include "Instruction.hpp"
#include "isa.hpp"
#include "Variable.hpp"
#include "BaseTranslationAxis.hpp"
#include "Logger.hpp"
#include "menuIdentifier.hpp"

#include <fstream>
#include <cmath>
#include <exception>
#include <mutex>

#include <QDoubleSpinBox>
#include <QSlider>
#include <QButtonGroup>

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
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

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

        std::vector<Instruction> programStack_;
        std::mutex interpreterLock_;
    bool learningTrajectory_ = false;
    bool emgStop_ = false;
    bool programInLoop_ = false;

    public slots:
        void changeMenu(int button_id);
        void toggleLearnBar();                        // hides or shows the learn bar
        void toggleCameraBar();                       // hides or shows the camera bar
        void updateAxes();                             // prints the axes onto the display
        void command();                                 // issues the commands to the axes

        void learn();                                   // go to learn mode
        void addStep();                                // memorize coordinates for direct learning
        void removeStep();                             // remove coordinate from direct learning
        void followLearnedPath();                             // execute the memorized coordinates
        void learnTrajectory();
        void saveProgramToDisk();
        void changeToLoop();

        void halt();
        void capture();

        void followColor();
        void loadProgram();
        void startFollowRed();
        void startFollowBlue();
        void startFollowGreen();
        void stopFollow();
        void jog();
        void initializeInterpreterThread();
        void checkIfFiledialog();

        void updateStick();
        //void poll_joystick();
        
        void rasmInterpreter();
        void cameraRestarter();
        bool joystickHotplugDetect();
        void toggleJog();


        

    //private:
    public:

        Ui::MainWindow *ui_;                             // user interface

        void setupApplication();
        void initTimers();
        void initButtons();
        void initSliders() const;
        void initCamera();

        void setButtonColor(int index);

        bool learnBarState_ = HIDDEN;                  // monitors the state of the bars
        bool cameraBarState_ = HIDDEN;
        void setLearnBarVisibility(bool state);
        void setCameraBarVisibility(bool state);
        bool jogging_ = false;
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
        QButtonGroup menuButtons_;
        std::vector<Instruction> manualProgram_;
        cv::Mat prev_;



        std::vector<QDoubleSpinBox*> increments_;
        std::vector<QSlider*> sliders_;
        std::vector<QPushButton*> buttons_;

        bool interruptFlag_ = false;


    bool checkIfMovementNecessary();
};

#endif // ROBOTDASHBOARD_MAINWINDOW_HPP
