#pragma once

#include "Arm_lib.hpp"

#ifdef __ARM_NEON
#include <arm_neon.h>
#else
typedef float float32_t;
#endif

#include <chrono>

class RobotArm : public ArmDevice
{
    public:

        RobotArm();
        const float translations[5] = { 74, 29.5, 82.85, 82.85, 71.5 };
        

       
        void rotateX(uint8_t num, float32_t* target );              // apply rotation matrix on the X axis
        void rotateY(uint8_t num, float32_t* target);               // apply rotation matrix on the Y axis
        void rotateZ(uint8_t num, float32_t* target);               // apply rotation matrix on the Z axis

        void translateX(uint8_t num, float32_t* target );           // apply translation matrix on the X axis
        void translateY(uint8_t num, float32_t* target);            // apply translation matrix on the Y axis
        void translateZ(uint8_t num, float32_t* target);            // apply translation matrix on the Z axis
        bool executing = false;

        void matrixMultiply(float32_t* T1, float32_t* T2, float32_t* T);                 // do matrix multiply using ARM NEON
        void matrixMultiplyCompatible(float32_t *A, float32_t *B, float32_t *C);                      // do matrix multipy using iterative method
        void printMatrix(float32_t*);                                                  // print a matrix

        void calculateEndEffector(float32_t* target);                                 // calculate the end effector using direct kinematics
        void goHome();                                                           // move to the home position

        std::vector<float32_t> homePosition_;
         std::vector<float32_t> currentPosition_;
        std::vector<float32_t> endPosition_;
        std::vector<float32_t> startPosition_;
        
        uint16_t timeFactor;
        std::chrono::time_point<std::chrono::system_clock> moveStartTime_;
        std::chrono::time_point<std::chrono::system_clock> moveCurrentTime_;

        void getCurrentPosition();
        void setDestination(std::vector<float> destination);
        void setTimeMod(int ms);
        bool checkCollision();
        bool moving = false;
        
    friend class MainWindow;
};