#include <RobotArm.hpp>
#include <iostream> // TODO: REMOVE

RobotArm::RobotArm() : ArmDevice() {
    homePosition_.reserve(6);
    homePosition_ = {90, 90, 90, 180, 90, 90};
    //homePosition_ = {90, 135, 180, 40, 90, 90};
    timeFactor_ = 1000;
}

#pragma region Matrix

#ifdef __ARM_NEON

void RobotArm::matrixMultiply(std::array<float32_t, 17> &t1, std::array<float32_t, 17> &t2, std::array<float32_t, 17> &t3) {

    // these are the columns A
    float32x4_t t_10 = vld1q_f32(t1.data());
    float32x4_t t_11 = vld1q_f32(t1.data() + 4);
    float32x4_t t_12 = vld1q_f32(t1.data() + 8);
    float32x4_t t_13 = vld1q_f32(t1.data() + 12);


    // these are the columns C
    float32x4_t t_30 = vmovq_n_f32(0);
    float32x4_t t_31 = vmovq_n_f32(0);
    float32x4_t t_32 = vmovq_n_f32(0);
    float32x4_t t_33 = vmovq_n_f32(0);


    // Multiply accumulate in 4x1 blocks, i.e. each column in C
    float32x4_t t_20 = vld1q_f32(t2.data());
    t_30 = vfmaq_laneq_f32(t_30, t_10, t_20, 0);
    t_30 = vfmaq_laneq_f32(t_30, t_11, t_20, 1);
    t_30 = vfmaq_laneq_f32(t_30, t_12, t_20, 2);
    t_30 = vfmaq_laneq_f32(t_30, t_13, t_20, 3);
    vst1q_f32(t3.data(), t_30);

    float32x4_t t_21 = vld1q_f32(t2.data() + 4);
    t_31 = vfmaq_laneq_f32(t_31, t_10, t_21, 0);
    t_31 = vfmaq_laneq_f32(t_31, t_11, t_21, 1);
    t_31 = vfmaq_laneq_f32(t_31, t_12, t_21, 2);
    t_31 = vfmaq_laneq_f32(t_31, t_13, t_21, 3);
    vst1q_f32(t3.data() + 4, t_31);

    float32x4_t t_22 = vld1q_f32(t2.data() + 8);
    t_32 = vfmaq_laneq_f32(t_32, t_10, t_22, 0);
    t_32 = vfmaq_laneq_f32(t_32, t_11, t_22, 1);
    t_32 = vfmaq_laneq_f32(t_32, t_12, t_22, 2);
    t_32 = vfmaq_laneq_f32(t_32, t_13, t_22, 3);
    vst1q_f32(t3.data() + 8, t_32);

    float32x4_t t_23 = vld1q_f32(t2.data() + 12);
    t_33 = vfmaq_laneq_f32(t_33, t_10, t_23, 0);
    t_33 = vfmaq_laneq_f32(t_33, t_11, t_23, 1);
    t_33 = vfmaq_laneq_f32(t_33, t_12, t_23, 2);
    t_33 = vfmaq_laneq_f32(t_33, t_13, t_23, 3);
    vst1q_f32(t3.data() + 12, t_33);
}

#else

void RobotArm::matrixMultiply(std::array<float32_t, 17> &t1, std::array<float32_t, 17> &t2, std::array<float32_t, 17> &t3) {
    matrixMultiplyCompatible(t1, t2, t3);
}

#endif

void RobotArm::matrixMultiplyCompatible(std::array<float32_t, 17> &a, std::array<float32_t, 17> &b, std::array<float32_t, 17> &c) {
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

void RobotArm::rotateX(uint8_t num, std::array<float32_t, 17> &target) {
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

void RobotArm::rotateY(uint8_t num, std::array<float32_t, 17> &target) {
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

void RobotArm::rotateZ(uint8_t num, std::array<float32_t, 17> &target) {
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

[[maybe_unused]] void RobotArm::translateX(uint8_t num, std::array<float32_t, 17> &target) {
    target[12] = this->translations_[num - 1];
}

[[maybe_unused]] void RobotArm::translateY(uint8_t num, std::array<float32_t, 17> &target) {
    target[13] = this->translations_[num - 1];
}

[[maybe_unused]] void RobotArm::translateZ(uint8_t num, std::array<float32_t, 17> &target) {
    target[14] = this->translations_[num - 1];
}

void RobotArm::calculateEndEffector(std::array<float32_t, 17> & target) {


    std::array<std::array<float32_t, 17>, 5> transformation_matrices =
            {{
                     {{0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 1}},

                     {{0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 1}},

                     {{0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 1}},

                     {{0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 1}},

                     {{0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 0,
                       0, 0, 0, 1}},
             }};


    std::array<std::array<float32_t, 17>, 5> steps = {0};

    translateZ(1, transformation_matrices.at(0));
    rotateZ(1, transformation_matrices.at(0));

    translateZ(2, transformation_matrices.at(1));
    rotateY(2, transformation_matrices.at(1));

    translateZ(3, transformation_matrices.at(2));
    rotateY(3, transformation_matrices.at(2));

    translateZ(4, transformation_matrices.at(3));
    rotateY(4, transformation_matrices.at(3));

    translateX(5, transformation_matrices.at(4));
    rotateX(5, transformation_matrices.at(4));

    matrixMultiply(transformation_matrices[0], transformation_matrices[1], target);

    matrixMultiply(transformation_matrices[0], transformation_matrices[1], steps[0]);
    matrixMultiply(steps[0], transformation_matrices[2], steps[1]);
    matrixMultiply(steps[1], transformation_matrices[3], steps[2]);
    matrixMultiply(steps[2], transformation_matrices[4], target);

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

    moving_ = true;
    moveStartTime_ = std::chrono::system_clock::now();
    startPosition_ = currentPosition_;
    timeFactor_ = 3000;
    servoWrite6(endPosition_.data(), timeFactor_);

    for (int i = 0; i < 6; i++)
        endPosition_.at(i) -= homePosition_.at(i);

}

[[maybe_unused]] void RobotArm::setTimeMod(int ms) {
    if (!moving_) {
        timeFactor_ = ms;
    } else {
        throw std::runtime_error("Attempt to set time factor before movement ended!");
    }
}

std::vector<float32_t> loggerRow1;
std::vector<float32_t> loggerRow2;
std::vector<float32_t> loggerRow3;
std::vector<float32_t> loggerRow4;

#include <fstream>

int counter = 0;

auto RobotArm::checkCollision() -> bool {
    /*std::cout << loggerRow1.size() << '\n';
    std::cout.flush();
    if (loggerRow1.size() == 300)
    {
        std::ofstream fout(std::string("/home/pi/") + std::to_string(counter) + std::string(".csv"));

        fout << "Pozitie curenta ,Pozitie estimata,Timp petrecut in cursa\n";
        for (int i = 0; i <loggerRow1.size(); i++)
        {
            //fout << loggerRow4.at(i) << "," <<loggerRow1.at(i) << "," << loggerRow2.at(i) << "," << loggerRow3.at(i) << '\n';
            fout << loggerRow1.at(i) << "," <<loggerRow2.at(i) << "," << loggerRow3.at(i) << std::endl; //<< "," << loggerRow3.at(i) << '\n';
        }
        fout.close();
        counter++;
        loggerRow1.clear();
        loggerRow2.clear();
        loggerRow3.clear();
        loggerRow4.clear();
        //throw std::runtime_error("HALTED AT REQ'D POINT");
    }*/

    switch ((int) moving_) {
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
            // conclusion, to determine position_ at a given moment, we must do
            // (distance / time_factor) * moment, thus getting an offset.
            // we add said offset to our current position_, and get our projected position_.
            timeFactor_ = 3000;

            for (int i = 0;
                 i < 1; i++) { //|                distance                  |   timeFactor_  |        time         |
                float offset = ((float) (endPosition_.at(i) - startPosition_.at(i)) / (float) timeFactor_) *
                               (float) milliseconds.count();

                float projected_position = offset + startPosition_.at(i) + homePosition_.at(i);

                if (milliseconds.count() <= timeFactor_ + 150) {
                    loggerRow1.push_back((float) currentPosition_.at(0) + (float) homePosition_.at(i));
                    loggerRow2.push_back((float) projected_position);
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
