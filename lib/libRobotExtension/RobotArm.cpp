#include <RobotArm.hpp>
#include <iostream> // TODO: REMOVE

RobotArm::RobotArm() : ArmDevice()
{
    homePosition_.reserve(6);
    homePosition_ = {90, 90, 90, 180, 90, 90};
}
#pragma region Matrix

#ifdef __ARM_NEON
void RobotArm::matrixMultiply(float32_t *T1, float32_t *T2, float32_t *T3)
{

    // these are the columns A
    float32x4_t T1_0;
    float32x4_t T1_1;
    float32x4_t T1_2;
    float32x4_t T1_3;

    // these are the columns B
    float32x4_t T2_0;
    float32x4_t T2_1;
    float32x4_t T2_2;
    float32x4_t T2_3;

    // these are the columns C
    float32x4_t T3_0;
    float32x4_t T3_1;
    float32x4_t T3_2;
    float32x4_t T3_3;

    T1_0 = vld1q_f32(T1);
    T1_1 = vld1q_f32(T1 + 4);
    T1_2 = vld1q_f32(T1 + 8);
    T1_3 = vld1q_f32(T1 + 12);

    // Zero accumulators for C values
    T3_0 = vmovq_n_f32(0);
    T3_1 = vmovq_n_f32(0);
    T3_2 = vmovq_n_f32(0);
    T3_3 = vmovq_n_f32(0);

    // Multiply accumulate in 4x1 blocks, i.e. each column in C
    T2_0 = vld1q_f32(T2);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_0, T2_0, 0);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_1, T2_0, 1);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_2, T2_0, 2);
    T3_0 = vfmaq_laneq_f32(T3_0, T1_3, T2_0, 3);
    vst1q_f32(T3, T3_0);

    T2_1 = vld1q_f32(T2 + 4);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_0, T2_1, 0);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_1, T2_1, 1);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_2, T2_1, 2);
    T3_1 = vfmaq_laneq_f32(T3_1, T1_3, T2_1, 3);
    vst1q_f32(T3 + 4, T3_1);

    T2_2 = vld1q_f32(T2 + 8);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_0, T2_2, 0);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_1, T2_2, 1);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_2, T2_2, 2);
    T3_2 = vfmaq_laneq_f32(T3_2, T1_3, T2_2, 3);
    vst1q_f32(T3 + 8, T3_2);

    T2_3 = vld1q_f32(T2 + 12);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_0, T2_3, 0);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_1, T2_3, 1);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_2, T2_3, 2);
    T3_3 = vfmaq_laneq_f32(T3_3, T1_3, T2_3, 3);
    vst1q_f32(T3 + 12, T3_3);
}
#else
void RobotArm::matrixMultiply(float32_t *T1, float32_t *T2, float32_t *T3)
{
    matrixMultiplyCompatible(T1, T2, T3);
}
#endif

void RobotArm::matrixMultiplyCompatible(float32_t *A, float32_t *B, float32_t *C)
{
    int n = 4, m = 4, k = 4;
    for (int i_idx = 0; i_idx < n; i_idx++)
    {
        for (int j_idx = 0; j_idx < m; j_idx++)
        {
            C[n * j_idx + i_idx] = 0;
            for (int k_idx = 0; k_idx < k; k_idx++)
            {
                C[n * j_idx + i_idx] += A[n * k_idx + i_idx] * B[k * j_idx + k_idx];
            }
        }
    }
}

void RobotArm::rotateX(uint8_t num, float32_t *target)
{
    float32_t phi = (this->currentPosition_[num - 1]) * __RAD__;
    target[0] = 1;
    target[4] = 0;
    target[8] = 0;
    target[1] = 0;
    target[5] = cos(phi);
    target[9] = -sin(phi);
    target[2] = 0;
    target[6] = sin(phi);
    target[10] = cos(phi);
}

void RobotArm::rotateY(uint8_t num, float32_t *target)
{
    float32_t phi = (this->currentPosition_[num - 1]) * __RAD__;
    target[0] = cos(phi);
    target[4] = 0;
    target[8] = sin(phi);
    target[1] = 0;
    target[5] = 1;
    target[9] = 0;
    target[2] = -sin(phi);
    target[6] = 0;
    target[10] = cos(phi);
}

void RobotArm::rotateZ(uint8_t num, float32_t *target)
{
    float32_t phi = (this->currentPosition_[num - 1]) * __RAD__;
    target[0] = cos(phi);
    target[4] = -sin(phi);
    target[8] = 0;
    target[1] = sin(phi);
    target[5] = cos(phi);
    target[9] = 0;
    target[2] = 0;
    target[6] = 0;
    target[10] = 1;
}

void RobotArm::translateX(uint8_t num, float32_t *target)
{
    target[12] = this->translations[num - 1];
}

void RobotArm::translateY(uint8_t num, float32_t *target)
{
    target[13] = this->translations[num - 1];
}

void RobotArm::translateZ(uint8_t num, float32_t *target)
{
    target[14] = this->translations[num - 1];
}

void RobotArm::calculateEndEffector(float32_t *target)
{
    float32_t Transformation_Matrices[][16] =
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

    translateZ(1, Transformation_Matrices[0]);
    rotateZ(1, Transformation_Matrices[0]);

    translateZ(2, Transformation_Matrices[1]);
    rotateY(2, Transformation_Matrices[1]);

    translateZ(3, Transformation_Matrices[2]);
    rotateY(3, Transformation_Matrices[2]);

    translateZ(4, Transformation_Matrices[3]);
    rotateY(4, Transformation_Matrices[3]);

    translateX(5, Transformation_Matrices[4]);
    rotateX(5, Transformation_Matrices[4]);

    matrixMultiply(Transformation_Matrices[0], Transformation_Matrices[1], target);

    matrixMultiply(Transformation_Matrices[0], Transformation_Matrices[1], steps[0]);
    matrixMultiply(steps[0], Transformation_Matrices[2], steps[1]);
    matrixMultiply(steps[1], Transformation_Matrices[3], steps[2]);
    matrixMultiply(steps[2], Transformation_Matrices[4], target);

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

void RobotArm::printMatrix(float32_t *M)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", M[j * 4 + i]);
        }
        printf("\n");
    }
    printf("\n");
}
#pragma endregion

void RobotArm::goHome()
{
    this->toggleTorque(true);
    this->servo_write6(homePosition_.data(), 1000);
    usleep(1000);
}
float move = 0;
bool sign = 1;
void RobotArm::getCurrentPosition()
{
    float32_t *data = servo_readall(); // read the values from all of the servos

    for (int i = 0; i < 6; i++)
    {
        switch (i + 1)
        {
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
void RobotArm::setDestination(std::vector<float> new_destination)
{
    endPosition_ = std::vector<float>(new_destination.cbegin() + 1, new_destination.cend());
    for(int i = 0 ; i < 6; i++)
        endPosition_.at(i) -= homePosition_.at(i);
    moving = true;
    moveStartTime_ = std::chrono::system_clock::now();
    startPosition_ = currentPosition_;
    
}
void RobotArm::setTimeMod(int ms)
{
    if (!moving)
    {
        timeFactor = ms;
    }
    else
    {
        throw std::runtime_error("Attempt to set time factor before movement ended!");
    }
}

std::vector<float32_t> logger_row1;
std::vector<float32_t> logger_row2;
std::vector<float32_t> logger_row3;

#include <fstream>

bool RobotArm::checkCollision()
{
    std::cout << logger_row1.size() << '\n';
    std::cout.flush();
    if (logger_row1.size() == 100)
    {
        std::ofstream fout("output.csv");

        fout << "ACTUAL,PROJECTED,PERCENT\n";
        for (int i = 0; i < 100; i++)
        {
            fout << logger_row1.at(i) << "," << logger_row2.at(i) << "," << logger_row3.at(i) << '\n';
        }
        fout.close();
        throw std::runtime_error("HALTED AT REQ'D POINT");
    }

    switch (moving)
    {
    case true:
    {
        moveCurrentTime_ = std::chrono::system_clock::now();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(moveCurrentTime_ - moveStartTime_);

        // time_factor = 1000
        // distance = end - start
        // debate

        // we must go from start to end in time_factor ms
        // therefore
        // we must be travelling at ( distance / time_factor ) deg / ms
        // conclusion, to determine position at a given moment, we must do
        // (distance / time_factor) * moment, thus getting an offset.
        // we add said offset to our current position, and get our projected position.
        timeFactor = 450;

        for (int i = 0; i < 1; i++)
        { //|                distance                  |   timeFactor  |        time         |
            float offset = ((endPosition_.at(i) - startPosition_.at(i)) / timeFactor) * milliseconds.count();
            float projectedPosition = offset + startPosition_.at(i);

            if (milliseconds.count() <= timeFactor)
            {
                logger_row1.push_back(currentPosition_.at(0) + homePosition_.at(i));
                logger_row2.push_back(projectedPosition);
                logger_row3.push_back((float)milliseconds.count() / (float)timeFactor * 100.0f);
            }
        }
        break;
    }
    default:
        break;
    }

    return false;
}
