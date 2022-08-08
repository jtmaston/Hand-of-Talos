#pragma once

#include "Arm_lib.hpp"

#ifdef __ARM_NEON
#include <arm_neon.h>
#else
typedef float float32_t;
#endif

class RobotArm : public ArmDevice
{
    public:

        RobotArm();
        //const float translations[5] = { 77, 28, 90, 90, 73 };
        //const float translations[5] = { 86.18, 34.75, 83.4, 83.4, 83.4};
        const float translations[5] = { 74, 29.5, 82.85, 82.85, 71.5 };


        std::vector<float32_t> angles;
        void rotateX(uint8_t num, float32_t* target );              // apply rotation matrix on the X axis
        void rotateY(uint8_t num, float32_t* target);               // apply rotation matrix on the Y axis
        void rotateZ(uint8_t num, float32_t* target);               // apply rotation matrix on the Z axis

        void translateX(uint8_t num, float32_t* target );           // apply translation matrix on the X axis
        void translateY(uint8_t num, float32_t* target);            // apply translation matrix on the Y axis
        void translateZ(uint8_t num, float32_t* target);            // apply translation matrix on the Z axis
        bool executing = false;

        void neon_multiply(float32_t* T1, float32_t* T2, float32_t* T);                 // do matrix multiply using ARM NEON
        void c_multiply(float32_t *A, float32_t *B, float32_t *C);                      // do matrix multipy using iterative method
        void print_matrix(float32_t*);                                                  // print a matrix

        void calculate_end_effector(float32_t* target);                                 // calculate the end effector using direct kinematics
        void go_home();                                                           // move to the home position

        //void servo_write6(float angles[6], uint16_t time);

        std::vector<float32_t> home_position;
        
    friend class MainWindow;
};