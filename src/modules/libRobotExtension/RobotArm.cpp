#include <RobotArm.hpp>

#ifdef __ARM_NEON
void RobotArm::neonMultiply(const float32_t *t1, const float32_t *t2, float32_t *t3) {

        // these are the columns A
        float32x4_t t_1_0 = vld1q_f32(t1);;
        float32x4_t t_1_1 = vld1q_f32(t1 + 4);
        float32x4_t t_1_2 = vld1q_f32(t1 + 8);
        float32x4_t t_1_3 = vld1q_f32(t1 + 12);
        
        // these are the columns B
        float32x4_t t_2_0 = {0.0f};
        float32x4_t t_2_1 = {0.0f};
        float32x4_t t_2_2 = {0.0f};
        float32x4_t t_2_3 = {0.0f};
        
        // these are the columns C
        float32x4_t t_3_0 = {0.0f};
        float32x4_t t_3_1 = {0.0f};
        float32x4_t t_3_2 = {0.0f};
        float32x4_t t_3_3 = {0.0f};


        // Zero accumulators for C values
        t_3_0 = vmovq_n_f32(0);
        t_3_1 = vmovq_n_f32(0);
        t_3_2 = vmovq_n_f32(0);
        t_3_3 = vmovq_n_f32(0);
        
        // Multiply accumulate in 4x1 blocks, i.e. each column in C
        t_2_0 = vld1q_f32(t2);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_0, t_2_0, 0);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_1, t_2_0, 1);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_2, t_2_0, 2);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_3, t_2_0, 3);
        vst1q_f32(t3, t_3_0);

        t_2_1 = vld1q_f32(t2 + 4);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_0, t_2_1, 0);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_1, t_2_1, 1);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_2, t_2_1, 2);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_3, t_2_1, 3);
        vst1q_f32(t3 + 4, t_3_1);

        t_2_2 = vld1q_f32(t2 + 8);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_0, t_2_2, 0);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_1, t_2_2, 1);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_2, t_2_2, 2);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_3, t_2_2, 3);
        vst1q_f32(t3 + 8, t_3_2);

        t_2_3 = vld1q_f32(t2 + 12);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_0, t_2_3, 0);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_1, t_2_3, 1);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_2, t_2_3, 2);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_3, t_2_3, 3);
        vst1q_f32(t3 + 12, t_3_3);
}
#else
#warning "NEON not supported by platform, falling back to compat mode."
void RobotArm::neonMultiply(const float32_t *t1, const float32_t *t2, float32_t *c)
{
    int n = 4, m = 4, k = 4;
    for (int i_idx = 0; i_idx < n; i_idx++)
    {
        for (int j_idx = 0; j_idx < m; j_idx++)
        {
            c[n * j_idx + i_idx] = 0;
            for (int k_idx = 0; k_idx < k; k_idx++)
            {
                c[n * j_idx + i_idx] += t1[n * k_idx + i_idx] * t2[k * j_idx + k_idx];
            }
        }
    }
}
#endif




void RobotArm::rotateX(uint8_t num, float32_t* target)
{
    float32_t phi = ( this -> angles_[num - 1] ) * RAD;
    target[0] = 1; target[4] = 0; target[8] = 0;
    target[1] = 0; target[5] = cos ( phi ); target[9] = -sin ( phi );
    target[2] = 0; target[6] = sin ( phi ); target[10] = cos ( phi );
}

void RobotArm::rotateY(uint8_t num, float32_t* target)
{
    float32_t phi = ( this -> angles_[num - 1] ) * RAD;
    target[0] = cos ( phi ); target[4] = 0; target[8] = sin ( phi );
    target[1] = 0; target[5] = 1; target[9] = 0;
    target[2] =-sin ( phi ); target[6] = 0; target[10] = cos ( phi );
}

void RobotArm::rotateZ(uint8_t num, float32_t* target)
{
    float32_t phi = ( this -> angles_[num - 1] ) * RAD ;
    target[0] = cos ( phi ); target[4] = -sin ( phi ); target[8] = 0;
    target[1] = sin ( phi ); target[5] = cos ( phi ); target[9] = 0;
    target[2] = 0; target[6] = 0; target[10] = 1;
}

void RobotArm::translateX(uint8_t num, float32_t* target )
{
    target[12] =  this -> translations_[num - 1 ];
}

void RobotArm::translateY(uint8_t num, float32_t* target)
{
    target[13] =  this -> translations_[num - 1 ];
}

void RobotArm::translateZ(uint8_t num, float32_t* target)
{
    target[14] = this -> translations_[num - 1 ];
}

void RobotArm::calculateEndEffector(std::array<float, 16> target)
{   
    float32_t transformation_matrices[][16] =
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

    float32_t steps[5][16] = { 0 };

    translateZ(1, transformation_matrices[0]);
    rotateZ(1, transformation_matrices[0]);

    translateZ(2, transformation_matrices[1]);
    rotateY(2, transformation_matrices[1]);

    translateZ(3, transformation_matrices[2]);
    rotateY(3, transformation_matrices[2]);

    translateZ(4, transformation_matrices[3]);
    rotateY(4, transformation_matrices[3]);

    translateX(5, transformation_matrices[4]);
    rotateX(5, transformation_matrices[4]);

    neonMultiply(transformation_matrices[0], transformation_matrices[1], target.data());

    neonMultiply(transformation_matrices[0], transformation_matrices[1], steps[0]);
    neonMultiply(steps[0], transformation_matrices[2], steps[1]);
    neonMultiply(steps[1], transformation_matrices[3], steps[2]);
    neonMultiply(steps[2], transformation_matrices[4], target.data());

    /*float32_t step1[16];
    neon_multiply(T1, T2, step1);
    //print_matrix(step1);

    float32_t step2[16];
    neon_multiply(step1, T3, step2);
    //print_matrix(step2);

    float32_t step3[16];
    neon_multiply(step2, T4, step3);
    //printMatrix(step3);
    neonMultiply(step3, T4, target);*/

}


void RobotArm::printMatrix(float32_t * m)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", m[j * 4 + i]);
        }
        printf("\n");
    }
    printf("\n");
}

