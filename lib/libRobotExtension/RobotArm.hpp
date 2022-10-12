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

    const float translations_[5] = {74, 29.5, 82.85, 82.85, 71.5};


    void rotateX(uint8_t num, std::array<float32_t, 17> &target);               // apply rotation matrix on the X axis
    void rotateY(uint8_t num, std::array<float32_t, 17> &target);               // apply rotation matrix on the Y axis
    void rotateZ(uint8_t num, std::array<float32_t, 17> &target);               // apply rotation matrix on the Z axis

    void
    translateX(uint8_t num, std::array<float32_t, 17> &target);            // apply translation matrix on the X axis
    void translateY(uint8_t num,
                    std::array<float32_t, 17> &target);                 // apply translation matrix on the Y axis
    void
    translateZ(uint8_t num, std::array<float32_t, 17> &target);            // apply translation matrix on the Z axis

    static void
    matrixMultiply(std::array<float32_t, 17> &t1, std::array<float32_t, 17> &t2,
                   std::array<float32_t, 17> &t3); // do matrix multiply using ARM NEON

    static void matrixMultiplyCompatible(std::array<float32_t, 17> &a, std::array<float32_t, 17> &b,
                                         std::array<float32_t, 17> &c);                      // do matrix multiply using iterative method
    [[maybe_unused]] static void
    printMatrix(float32_t *);                                                  // print a matrix

    void calculateEndEffector(
            std::array<float32_t, 17> &target);                                 // calculate the end effector using direct kinematics
    void goHome();                                                           // move to the home position_

    std::vector<float32_t> homePosition_;
    std::vector<float32_t> currentPosition_;
    std::vector<float32_t> endPosition_;
    std::vector<float32_t> startPosition_;

    uint16_t timeFactor_;
    std::chrono::time_point<std::chrono::system_clock> moveStartTime_;
    std::chrono::time_point<std::chrono::system_clock> moveCurrentTime_;

    void getCurrentPosition();

    void setDestination(const std::vector<float> &newDestination);

    [[maybe_unused]] void setTimeMod(int ms);

    bool checkCollision();

    bool moving_ = false;

    friend class MainWindow;
};