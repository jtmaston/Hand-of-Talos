#include <lib/RobotArm.hpp>

RobotArm::RobotArm() : ArmDevice()
{
    home_position.reserve(6);
    home_position = { 90, 135, 45, 0, 90, 90 };
}

void RobotArm::neon_multiply(float32_t *T1, float32_t *T2, float32_t *T3) {

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
        T1_1 = vld1q_f32(T1+4);
        T1_2 = vld1q_f32(T1+8);
        T1_3 = vld1q_f32(T1+12);
        
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
        
        T2_1 = vld1q_f32(T2+4);
        T3_1 = vfmaq_laneq_f32(T3_1, T1_0, T2_1, 0);
        T3_1 = vfmaq_laneq_f32(T3_1, T1_1, T2_1, 1);
        T3_1 = vfmaq_laneq_f32(T3_1, T1_2, T2_1, 2);
        T3_1 = vfmaq_laneq_f32(T3_1, T1_3, T2_1, 3);
        vst1q_f32(T3+4, T3_1);
        
        T2_2 = vld1q_f32(T2+8);
        T3_2 = vfmaq_laneq_f32(T3_2, T1_0, T2_2, 0);
        T3_2 = vfmaq_laneq_f32(T3_2, T1_1, T2_2, 1);
        T3_2 = vfmaq_laneq_f32(T3_2, T1_2, T2_2, 2);
        T3_2 = vfmaq_laneq_f32(T3_2, T1_3, T2_2, 3);
        vst1q_f32(T3+8, T3_2);
        
        T2_3 = vld1q_f32(T2+12);
        T3_3 = vfmaq_laneq_f32(T3_3, T1_0, T2_3, 0);
        T3_3 = vfmaq_laneq_f32(T3_3, T1_1, T2_3, 1);
        T3_3 = vfmaq_laneq_f32(T3_3, T1_2, T2_3, 2);
        T3_3 = vfmaq_laneq_f32(T3_3, T1_3, T2_3, 3);
        vst1q_f32(T3+12, T3_3);
}

void RobotArm::c_multiply(float32_t *A, float32_t *B, float32_t *C)
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


void RobotArm::rotateX(uint8_t num, float32_t* target)
{
    float32_t phi = this -> angles[ num - 1] * __RAD__;
    target[0] = 1; target[4] = 0; target[8] = 0;
    target[1] = 0; target[5] = cos ( phi ); target[9] = -sin ( phi );
    target[2] = 0; target[6] = sin ( phi ); target[10] = cos ( phi );
}

void RobotArm::rotateY(uint8_t num, float32_t* target)
{
    float32_t phi = ( this -> angles[ num - 1] ) * __RAD__;
    target[0] = cos ( phi ); target[4] = 0; target[8] = sin ( phi );
    target[1] = 0; target[5] = 1; target[9] = 0;
    target[2] =-sin ( phi ); target[6] = 0; target[10] = cos ( phi );
}

void RobotArm::rotateZ(uint8_t num, float32_t* target)
{
    float32_t phi = ( this -> angles[ num - 1] ) * __RAD__ ;
    target[0] = cos ( phi ); target[4] = -sin ( phi ); target[8] = 0;
    target[1] = sin ( phi ); target[5] = cos ( phi ); target[9] = 0;
    target[2] = 0; target[6] = 0; target[10] = 1;
}

void RobotArm::translateX(uint8_t num, float32_t* target )
{
    target[12] =  this -> translations[ num - 1 ];
}

void RobotArm::translateY(uint8_t num, float32_t* target)
{
    target[13] =  this -> translations[ num - 1 ];
}

void RobotArm::translateZ(uint8_t num, float32_t* target)
{
    target[14] = this -> translations[ num - 1 ];
}

void RobotArm::calculate_end_effector(float32_t* target)
{   
    float32_t T1 [ 16 ] = { 0 };
    T1[15] = 1;
    translateZ(1, T1);
    rotateZ(1, T1);

    float32_t T2 [ 16 ] = { 0 };
    T2[15] = 1;
    translateX(2, T2);
    translateY(3, T2);
    translateZ(4, T2);
    rotateY(2, T2);
    //print_matrix(T2);

    //print_matrix(T2);

    float32_t T3 [ 16 ] = { 0 };
    T3[15] = 1;
    //translateX()
    translateY(5, T3);
    translateZ(6, T3);
    rotateY(3, T3);
    //print_matrix(T3);

    float32_t T4 [ 16 ] = { 0 };
    T4[15] = 1;
    translateX(7, T4);
    translateY(8, T4);
    rotateX(4, T4);

    float32_t T5 [ 16 ] = { 0 };
    T5[15] = 1;
    translateX(9, T5);
    rotateY(5, T5);

    float32_t T6 [ 16 ] = { 0 };
    T6[15] = 1;
    translateX(10, T6);
    rotateX(6, T6);

    float32_t T7 [ 16 ] = { 0 };
    T7[15] = 1;
    translateX(11, T7);
    rotateX(7, T7);

    //print_matrix(T7);

    float32_t step1[16];
    neon_multiply(T1, T2, step1);
    //print_matrix(step1);

    float32_t step2[16];
    neon_multiply(step1, T3, step2);
    //print_matrix(step2);

    float32_t step3[16];
    neon_multiply(step2, T4, step3);
    //print_matrix(step3);

    float32_t step4[16];
    neon_multiply(step3, T5, step4);
    //print_matrix(step4);
    float32_t step5[16];
    neon_multiply(step4, T6, step5);
    //print_matrix(step5);
    //float32_t step6[16];
    neon_multiply(step5, T7, target);

    //neon_multiply(step6, T7, target);
}


void RobotArm::print_matrix(float32_t *M)
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

void RobotArm::go_home()
{
    this -> toggleTorque(true);
    this -> servo_write6(home_position.data(), 1000);
    usleep(1000000);
    //toggleTorque(0);
}