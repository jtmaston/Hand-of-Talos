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
#include <QSlider>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <quirc.h>

#include <linux/videodev.h>

#include "Instruction.hpp"
#include "isa.hpp"
#include "Variable.hpp"
#include "BaseTranslationAxis.hpp"
#include "Image.hpp"

#include <fstream>
#include <exception>

#include <mutex>
#include "Logger.hpp"

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

         // FIXME: move somewhere else | FIXME: rename

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        int trackingDirection_ = 0;

        Rect redBoundingBox_;
        Rect greenBoundingBox_;
        Rect blueBoundingBox_;

        bool applicationIsRunning_;
        QString incomingFilename_;
        struct quirc *qrDecoder_;

        void goToHomePosition();
        VideoCapture *robotCamera_ = nullptr;
        
        std::vector<Instruction>* instructionQueue_;
        bool waitForMoveComplete_;

        void initPeripherals();
        void initSignals();
        void initDevice();

        QSlider* uiSliderArray_[6];


        bool learnModeActive_ = 0;

    public slots:
        void toggleLearnBar();                        // hides or shows the learn bar
        void toggleCameraBar();                       // hides or shows the camera bar
        void updateAxisDisplay();                             // prints the axes onto the display
        void moveToPosition();                                 // issues the commands to the axes

        void startLearnMode();                                   // go to learn mode
        void addStep();                                // memorize coordinates for direct learning
        void removeStep();                             // remove coordinate from direct learning
        void startFollowingPath();                             // execute the memorized coordinates

        void getFrame();

        void startColorTracking();
        void emergencyStop();
        void setTrackingColorRed();
        void setTrackingColorBlue();
        void setTrackingColorGreen();
        void stopTracking();

        void startJogging();
        void updateJoystickPosition();
        bool detectJoystickHotplug();
        
        void runProgram();
        void checkForFileIngress();
        
        
        void rasmInterpreter(const std::vector <float>, std::vector<Instruction>*, bool*);
        void cameraRestarter();
        
        void updateViewfinderFrame();
        void postprocessImage();

        // this is an *ugly* implementation, but we have to live with it, due to qt5's garbage slot mechanism

        void runGetCurrentPosition();
        void runCheckCollision();

        void crappyDelay(int ms);
        

    private:
        Ui::MainWindow *ui = nullptr;                             // user interface
        bool learnBarVisibility_ = HIDDEN;                  // monitors the state of the bars
        bool cameraBarVisibility = HIDDEN;

        void setLearnBarVisibility(bool state);
        void setCameraBarVisibility(bool state);
        void setAxisReadoutVisibility(bool state);
        bool runningProgram_ = false;
        bool runningLearnMode_ = false;
        bool hasFileOpen_ = false;

        QTimer *Scheduler_100ms_ = nullptr;                        // timers for different actions
        QTimer *Scheduler_16ms_ = nullptr;
        QTimer *Scheduler_500ms_ = nullptr;
        QTimer *Scheduler_10ms_ = nullptr;

        RobotArm dev_;
        BaseTranslationAxis translationAxis_;
        
        QGamepad* joystick_ = nullptr;
        QGamepadManager* gameplayManager_;


        QFuture<void> cameraThread_;
        QFuture<void> postProcessinThread_;
        QFuture<void> leanModeThread_;
        QFuture<void> progThread_;

        std::vector<Instruction> manualProgramStack_;
        cv::Mat incomingFrame_;

        std::mutex antiFreewheel_;
        std::mutex synchroMesh_;
        std::mutex displaySync_;


};

#endif // MAINWINDOW_H
