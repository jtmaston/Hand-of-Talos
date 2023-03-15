#include <RobotArm.hpp>

inline void RobotArm::rotateX(uint8_t num, TransformationMatrix &target) {
    float32_t phi = ( this -> angles_.at(num - 1) ) * RAD;
    target.at(0) = 1; target.at(4) = 0; target.at(8) = 0;
    target.at(1) = 0; target.at(5) = cos ( phi ); target.at(9) = -sin ( phi );
    target.at(2) = 0; target.at(6) = sin ( phi ); target.at(10) = cos ( phi );
}

inline void RobotArm::rotateY(uint8_t num, TransformationMatrix &target) {
    float32_t phi = ( this -> angles_.at(num - 1) ) * RAD;
    target.at(0) = cos ( phi ); target.at(4) = 0; target.at(8) = sin ( phi );
    target.at(1) = 0; target.at(5) = 1; target.at(9) = 0;
    target.at(2) =-sin ( phi ); target.at(6) = 0; target.at(10) = cos ( phi );
}

inline void RobotArm::rotateZ(uint8_t num, TransformationMatrix &target) {
    float32_t phi = ( this -> angles_.at(num - 1) ) * RAD ;
    target.at(0) = cos ( phi ); target.at(4) = -sin ( phi ); target.at(8) = 0;
    target.at(1) = sin ( phi ); target.at(5) = cos ( phi ); target.at(9) = 0;
    target.at(2) = 0; target.at(6) = 0; target.at(10) = 1;
}

inline void RobotArm::translateX(uint8_t num, TransformationMatrix &target) {
    target.at(12) =  this -> translations_.at(num - 1 );
}

inline void RobotArm::translateY(uint8_t num, TransformationMatrix &target) {
    target.at(13) =  this -> translations_.at(num - 1 );
}

inline void RobotArm::translateZ(uint8_t num, TransformationMatrix &target) {
    target.at(14) = this -> translations_.at(num - 1 );
}
#undef __ARM_NEON
#ifdef __ARM_NEON
void RobotArm::multiplyMatrix(const TransformationMatrix &t1, const TransformationMatrix &t2, TransformationMatrix &t3) {

        // these are the columns A
        float32x4_t t_1_0 = vld1q_f32(t1.data());
        float32x4_t t_1_1 = vld1q_f32(t1.data() + 4);
        float32x4_t t_1_2 = vld1q_f32(t1.data() + 8);
        float32x4_t t_1_3 = vld1q_f32(t1.data() + 12);

        // these are the columns B
        float32x4_t t_2_0;
        float32x4_t t_2_1;
        float32x4_t t_2_2;
        float32x4_t t_2_3;

        // these are the columns C
        float32x4_t t_3_0;
        float32x4_t t_3_1;
        float32x4_t t_3_2;
        float32x4_t t_3_3;


        // Zero accumulators for C values
        t_3_0 = vmovq_n_f32(0);
        t_3_1 = vmovq_n_f32(0);
        t_3_2 = vmovq_n_f32(0);
        t_3_3 = vmovq_n_f32(0);

        // Multiply accumulate in 4x1 blocks, i.e. each column in C
        t_2_0 = vld1q_f32(t2.data());
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_0, t_2_0, 0);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_1, t_2_0, 1);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_2, t_2_0, 2);
        t_3_0 = vfmaq_laneq_f32(t_3_0, t_1_3, t_2_0, 3);
        vst1q_f32(t3.data(), t_3_0);

        t_2_1 = vld1q_f32(t2.data() + 4);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_0, t_2_1, 0);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_1, t_2_1, 1);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_2, t_2_1, 2);
        t_3_1 = vfmaq_laneq_f32(t_3_1, t_1_3, t_2_1, 3);
        vst1q_f32(t3.data() + 4, t_3_1);

        t_2_2 = vld1q_f32(t2.data() + 8);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_0, t_2_2, 0);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_1, t_2_2, 1);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_2, t_2_2, 2);
        t_3_2 = vfmaq_laneq_f32(t_3_2, t_1_3, t_2_2, 3);
        vst1q_f32(t3.data() + 8, t_3_2);

        t_2_3 = vld1q_f32(t2.data() + 12);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_0, t_2_3, 0);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_1, t_2_3, 1);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_2, t_2_3, 2);
        t_3_3 = vfmaq_laneq_f32(t_3_3, t_1_3, t_2_3, 3);
        vst1q_f32(t3.data() + 12, t_3_3);
}
#else
#warning "NEON not supported by platform, falling back to compat mode."
void RobotArm::multiplyMatrix(const TransformationMatrix &t1, const TransformationMatrix &t2, TransformationMatrix &t3) {
    int n = 4, m = 4, k = 4;
    for (int i_idx = 0; i_idx < n; i_idx++)
    {
        for (int j_idx = 0; j_idx < m; j_idx++)
        {
            t3.at(n * j_idx + i_idx) = 0;
            for (int k_idx = 0; k_idx < k; k_idx++)
            {
                t3.at(n * j_idx + i_idx) += t1.at(n * k_idx + i_idx) * t2.at(k * j_idx + k_idx);
            }
        }
    }
}
#endif

void RobotArm::printMatrix(const TransformationMatrix & m) {
    printf("{");
    for (int i = 0; i < 4; i++)
    {
        printf("{");
        for (int j = 0; j < 4; j++)
        {
            printf("%.2f, ", m.at(j * 4 + i));
        }
        printf("}\n");
    }
    printf("}\n");
}

void RobotArm::calculateEndEffector(TransformationMatrix &target) {
    std::array<TransformationMatrix , 7> transformation_matrices=
            {
                    {{  0, 0, 0, 0,
                        0, 0, 0, 0,
                        0, 0, 0, 0,
                        0, 0, 0, 1
                     },
                     {0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 1
                     },
                     {0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 1
                     },
                     {0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 0,
                      0, 0, 0, 1
                     },
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
                      0, 0, 0, 1}
                    }
            };

    //translateZ(1, transformation_matrices.at(0));
    /*rotateZ(1, transformation_matrices.at(0));

    translateZ(2, transformation_matrices.at(1));
    rotateY(2, transformation_matrices.at(1));

    translateZ(3, transformation_matrices.at(2));
    rotateY(3, transformation_matrices.at(2));

    translateZ(4, transformation_matrices.at(3));
    rotateY(4, transformation_matrices.at(3));

    translateX(5, transformation_matrices.at(4));
    rotateX(5, transformation_matrices.at(4));

    multiplyMatrix(transformation_matrices.at(0), transformation_matrices.at(1), target);

    std::array<TransformationMatrix ,5> steps{};
    multiplyMatrix(transformation_matrices.at(0), transformation_matrices.at(1), steps.at(0));
    multiplyMatrix(steps.at(0), transformation_matrices.at(2), steps.at(1));
    multiplyMatrix(steps.at(1), transformation_matrices.at(3), steps.at(2));
    multiplyMatrix(steps.at(2), transformation_matrices.at(4), target);*/



    translateZ(1, transformation_matrices.at(0));
    rotateZ(1, transformation_matrices.at(0));

    translateZ(2, transformation_matrices.at(1));
    translateX(3, transformation_matrices.at(1));
    rotateY(2, transformation_matrices.at(1));

    translateZ(4, transformation_matrices.at(2));
    rotateY(3, transformation_matrices.at(2));

    translateZ(5, transformation_matrices.at(3));
    translateX(6, transformation_matrices.at(3));
    rotateX(4, transformation_matrices.at(3));

    translateX(7, transformation_matrices.at(4));
    rotateY(5, transformation_matrices.at(4));

    translateX(8, transformation_matrices.at(5));
    rotateX(6, transformation_matrices.at(5));

    std::array<TransformationMatrix ,7> steps{};
    multiplyMatrix(transformation_matrices.at(0), transformation_matrices.at(1), steps.at(0));
    multiplyMatrix(steps.at(0), transformation_matrices.at(2), steps.at(1));
    multiplyMatrix(steps.at(1), transformation_matrices.at(3), steps.at(2));
    multiplyMatrix(steps.at(2), transformation_matrices.at(4), steps.at(3));
    multiplyMatrix(steps.at(3), transformation_matrices.at(5), target);

}
