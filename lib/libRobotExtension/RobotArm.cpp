#include <RobotArm.hpp>
#include <iostream> // TODO: REMOVE

RobotArm::RobotArm() : ArmDevice() {
    homePosition_.reserve(6);
    homePosition_ = {90, 90, 90, 180, 90, 90};
    timeFactor = 1000;
}

#pragma region Matrix

#ifdef __ARM_NEON

void RobotArm::matrixMultiply(float32_t *t1, float32_t *t2, float32_t *t3) {

    // these are the columns A
    float32x4_t t10;
    float32x4_t t11;
    float32x4_t t12;
    float32x4_t t13;

    // these are the columns B
    float32x4_t t20;
    float32x4_t t21;
    float32x4_t t22;
    float32x4_t t23;

    // these are the columns C
    float32x4_t t30;
    float32x4_t t31;
    float32x4_t t32;
    float32x4_t t33;

    t10 = vld1q_f32(t1);
    t11 = vld1q_f32(t1 + 4);
    t12 = vld1q_f32(t1 + 8);
    t13 = vld1q_f32(t1 + 12);

    // Zero accumulators for C values
    t30 = vmovq_n_f32(0);
    t31 = vmovq_n_f32(0);
    t32 = vmovq_n_f32(0);
    t33 = vmovq_n_f32(0);

    // Multiply accumulate in 4x1 blocks, i.e. each column in C
    t20 = vld1q_f32(t2);
    t30 = vfmaq_laneq_f32(t30, t10, t20, 0);
    t30 = vfmaq_laneq_f32(t30, t11, t20, 1);
    t30 = vfmaq_laneq_f32(t30, t12, t20, 2);
    t30 = vfmaq_laneq_f32(t30, t13, t20, 3);
    vst1q_f32(t3, t30);

    t21 = vld1q_f32(t2 + 4);
    t31 = vfmaq_laneq_f32(t31, t10, t21, 0);
    t31 = vfmaq_laneq_f32(t31, t11, t21, 1);
    t31 = vfmaq_laneq_f32(t31, t12, t21, 2);
    t31 = vfmaq_laneq_f32(t31, t13, t21, 3);
    vst1q_f32(t3 + 4, t31);

    t22 = vld1q_f32(t2 + 8);
    t32 = vfmaq_laneq_f32(t32, t10, t22, 0);
    t32 = vfmaq_laneq_f32(t32, t11, t22, 1);
    t32 = vfmaq_laneq_f32(t32, t12, t22, 2);
    t32 = vfmaq_laneq_f32(t32, t13, t22, 3);
    vst1q_f32(t3 + 8, t32);

    t23 = vld1q_f32(t2 + 12);
    t33 = vfmaq_laneq_f32(t33, t10, t23, 0);
    t33 = vfmaq_laneq_f32(t33, t11, t23, 1);
    t33 = vfmaq_laneq_f32(t33, t12, t23, 2);
    t33 = vfmaq_laneq_f32(t33, t13, t23, 3);
    vst1q_f32(t3 + 12, t33);
}

#else
void RobotArm::matrixMultiply(float32_t *T1, float32_t *T2, float32_t *T3)
{
    matrixMultiplyCompatible(T1, T2, T3);
}
#endif

void RobotArm::matrixMultiplyCompatible(const float32_t *a, const float32_t *b, float32_t *c) {
    int n = 4, m = 4, o = 4;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            c[n * j + i] = 0;
            for (int k = 0; k < o; k++) {
                c[n * j + i] += a[n * k + i] * b[o * j + k];
            }
        }
    }
}

void RobotArm::rotateX(uint8_t num, float32_t *target) {
    float32_t phi = (this->currentPosition_[num - 1]) * (float) RAD;
    target[0] = 1;
    target[4] = 0;
    target[8] = 0;
    target[1] = 0;
    target[5] = (float) cos(phi);
    target[9] = (float) -sin(phi);
    target[2] = 0;
    target[6] = (float) sin(phi);
    target[10] = (float) cos(phi);
}

void RobotArm::rotateY(uint8_t num, float32_t *target) {
    float32_t phi = (this->currentPosition_[num - 1]) * (float) RAD;
    target[0] = (float) cos(phi);
    target[4] = 0;
    target[8] = (float) sin(phi);
    target[1] = 0;
    target[5] = 1;
    target[9] = 0;
    target[2] = (float) -sin(phi);
    target[6] = 0;
    target[10] = (float) cos(phi);
}

void RobotArm::rotateZ(uint8_t num, float32_t *target) {
    float32_t phi = (this->currentPosition_[num - 1]) * (float) RAD;
    target[0] = (float) cos(phi);
    target[4] = (float) -sin(phi);
    target[8] = 0;
    target[1] = (float) sin(phi);
    target[5] = (float) cos(phi);
    target[9] = 0;
    target[2] = 0;
    target[6] = 0;
    target[10] = 1;
}

[[maybe_unused]] void RobotArm::translateX(uint8_t num, float32_t *target) {
    target[12] = this->translations[num - 1];
}

[[maybe_unused]] void RobotArm::translateY(uint8_t num, float32_t *target) {
    target[13] = this->translations[num - 1];
}

[[maybe_unused]] void RobotArm::translateZ(uint8_t num, float32_t *target) {
    target[14] = this->translations[num - 1];
}

void RobotArm::calculateEndEffector(float32_t *target) {
    float32_t transformationMatrices[][16] =
            {
                    {0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1},
                    {0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1},
                    {0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1},
                    {0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1},
                    {0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 1},
            };

    float32_t steps[5][16] = {0};

    translateZ(1, transformationMatrices[0]);
    rotateZ(1, transformationMatrices[0]);

    translateZ(2, transformationMatrices[1]);
    rotateY(2, transformationMatrices[1]);

    translateZ(3, transformationMatrices[2]);
    rotateY(3, transformationMatrices[2]);

    translateZ(4, transformationMatrices[3]);
    rotateY(4, transformationMatrices[3]);

    translateX(5, transformationMatrices[4]);
    rotateX(5, transformationMatrices[4]);

    matrixMultiply(transformationMatrices[0], transformationMatrices[1], target);

    matrixMultiply(transformationMatrices[0], transformationMatrices[1], steps[0]);
    matrixMultiply(steps[0], transformationMatrices[2], steps[1]);
    matrixMultiply(steps[1], transformationMatrices[3], steps[2]);
    matrixMultiply(steps[2], transformationMatrices[4], target);

    /*float32_t step1[16];
    neon_multiply(T1, T2, step1);
    //print_matrix(step1);

    float32_t step2[16];
    neon_multiply(step1, T3, step2);
    //print_matrix(step2);

    float32_t step3[16];
    neon_multiply(step2, T4, step3);
    //print_matrix(step3);
    neon_multiply(step3, T4, target);*/
}

[[maybe_unused]] void RobotArm::printMatrix(float32_t *m) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", m[j * 4 + i]);
        }
        printf("\n");
    }
    printf("\n");
}

#pragma endregion

void RobotArm::goHome() {
    this->toggleTorque(true);
    this->servoWrite6(homePosition_.data(), 1000);
    usleep(1000);
}


void RobotArm::getCurrentPosition() {
    float32_t *data = servoReadall(); // read the values from all the servos

    for (int i = 0; i < 6; i++) {
        switch (i + 1) {
            case 1:
            case 6:
                currentPosition_[i] = data[i] - homePosition_[i];
                break;
            case 2:
            case 3:
                currentPosition_[i] = -(data[i] - homePosition_[i]);
                currentPosition_[i] = -(data[i] - homePosition_[i]);
                break;

            case 4:
                currentPosition_[i] = -data[i];
                break;

            case 5:
                currentPosition_[i] = data[i] - homePosition_[i];
                break;
        }
        // std::cout << angles[i] << " ";
    }
    // std::cout << '\n';
    delete data; // readall returns a dynamic pointer, so it must be deleted to prevent memory leaks
}

void RobotArm::setDestination(const std::vector<float> &newDestination) {
    endPosition_ = std::vector<float>(newDestination.cbegin() + 1, newDestination.cend());

    moving = true;
    moveStartTime_ = std::chrono::system_clock::now();
    startPosition_ = currentPosition_;
    timeFactor = 3000;
    servoWrite6(endPosition_.data(), timeFactor);

    for (int i = 0; i < 6; i++)
        endPosition_.at(i) -= homePosition_.at(i);

}

[[maybe_unused]] void RobotArm::setTimeMod(int ms) {
    if (!moving) {
        timeFactor = ms;
    } else {
        throw std::runtime_error("Attempt to set time factor before movement ended!");
    }
}

std::vector<float32_t> loggerRow1;
std::vector<float32_t> loggerRow2;
std::vector<float32_t> loggerRow3;
std::vector<float32_t> loggerRow4;

// #include <fstream>

bool RobotArm::checkCollision() {
    //std::cout << loggerRow1.size() << '\n';
    /*std::cout.flush();
    if (loggerRow1.size() == 100)
    {
        std::ofstream fout("output.csv");

        fout << "TARGET,ACTUAL,PROJECTED,PERCENT\n";
        for (int i = 0; i < 100; i++)
        {
            fout << loggerRow4.at(i) << "," <<loggerRow1.at(i) << "," << loggerRow2.at(i) << "," << loggerRow3.at(i) << '\n';
        }
        fout.close();
        throw std::runtime_error("HALTED AT REQ'D POINT");
    }*/

    switch ((int) moving) {
        case 1: {
            moveCurrentTime_ = std::chrono::system_clock::now();
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                    moveCurrentTime_ - moveStartTime_);

            // time_factor = 1000
            // distance = end - start
            // debate

            // we must go from start to end in time_factor ms
            // therefore
            // we must be travelling at ( distance / time_factor ) deg / ms
            // conclusion, to determine position at a given moment, we must do
            // (distance / time_factor) * moment, thus getting an offset.
            // we add said offset to our current position, and get our projected position.
            timeFactor = 3000;

            for (int i = 0;
                 i < 1; i++) { //|                distance                  |   timeFactor  |        time         |
                float offset = ((float) (endPosition_.at(i) - startPosition_.at(i)) / (float) timeFactor) *
                               (float) milliseconds.count();

                float projectedPosition = offset + startPosition_.at(i) + homePosition_.at(i);

                if (milliseconds.count() <= timeFactor) {
                    loggerRow1.push_back((float) currentPosition_.at(0) + (float) homePosition_.at(i));
                    loggerRow2.push_back((float) projectedPosition);
                    loggerRow3.push_back((float) milliseconds.count());
                    loggerRow4.push_back(endPosition_.at(i) + homePosition_.at(i));
                }
            }
            break;
        }
        default:
            break;
    }

    return false;
}
