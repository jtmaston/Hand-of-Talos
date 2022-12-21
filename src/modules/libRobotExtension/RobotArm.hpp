#pragma once

#include "Arm_lib.hpp"

#ifdef __ARM_NEON
#include <arm_neon.h>
#else
using float32_t = float;
#endif

#include <array>

class RobotArm : public ArmDevice
{
    public:
        const std::array<float, 5> translations_ = {74, 29.5, 82.85, 82.85, 71.5 };


        std::array<float, 6> angles_;
        void rotateX(uint8_t num, float32_t* target );              // apply rotation matrix on the X axis
        void rotateY(uint8_t num, float32_t* target);               // apply rotation matrix on the Y axis
        void rotateZ(uint8_t num, float32_t* target);               // apply rotation matrix on the Z axis

        void translateX(uint8_t num, float32_t* target );           // apply translation matrix on the X axis
        void translateY(uint8_t num, float32_t* target);            // apply translation matrix on the Y axis
        void translateZ(uint8_t num, float32_t* target);            // apply translation matrix on the Z axis
        bool executing_ = false;

        static void neonMultiply(const float32_t* t1, const float32_t* t2, float32_t* t3);                 // do matrix multiply using ARM NEON
        static void cMultiply(const float32_t *a, const float32_t *b, float32_t *c);                      // do matrix multipy using iterative method
        static void printMatrix(float32_t*);                                                  // print a matrix

        void calculateEndEffector(std::array<float, 16> target);                                 // calculate the end effector using direct kinematics         // move to the home position_

    friend class MainWindow;
};