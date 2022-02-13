#pragma once

#include "lib/libRobot/include/Arm_lib.hpp"

#include <arm_neon.h>

class RobotArm : public ArmDevice
{
    public:
        const float translations[13] = {104.5,  70, 65, 247.5,  -130, 360,  238.5, 65,  141.5, 39.5, 25.5};         // used in direct kinematics

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
    
    friend class MainWindow;
};