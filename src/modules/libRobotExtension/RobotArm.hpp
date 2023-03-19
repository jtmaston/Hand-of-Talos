#pragma once

#include "Arm_lib.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


#ifdef __ARM_NEON
#include <arm_neon.h>

#else
using float32_t = float;
#endif

#include <array>

using TransformationMatrix = std::array<float32_t, 16>;

class RobotArm : public ArmDevice
{
    public:
        //const std::array<float, 5> translations_ = {74, 29.5, 82.85, 82.85, 71.5 };
        const std::array<float, 8> translations_ = {80, 270, 100, 250, 130, 80, 170, 85 };


        std::array<float, 6> angles_;
        void rotateX(uint8_t num, TransformationMatrix& target );              // apply rotation matrix on the X axis
        void rotateY(uint8_t num, TransformationMatrix& target);               // apply rotation matrix on the Y axis
        void rotateZ(uint8_t num, TransformationMatrix& target);               // apply rotation matrix on the Z axis

        void translateX(uint8_t num, TransformationMatrix& target );           // apply translation matrix on the X axis
        void translateY(uint8_t num, TransformationMatrix& target);            // apply translation matrix on the Y axis
        void translateZ(uint8_t num, TransformationMatrix& target);            // apply translation matrix on the Z axis
        bool executing_ = false;

        static void multiplyMatrix(const TransformationMatrix& t1, const TransformationMatrix& t2, TransformationMatrix& t3);                 // do matrix multiply using ARM NEON
        static void printMatrix(const TransformationMatrix&);                                                  // print a matrix

        void calculateEndEffector(TransformationMatrix& target);                                 // calculate the end effector using direct kinematics         // move to the home position_
        void matrixToEuler(const cv::Mat& rotationMatrix, cv::Vec3d& eulerAngles);

    friend class MainWindow;
};