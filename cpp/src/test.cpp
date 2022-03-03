#include <eigen3/Eigen/Dense>
#include <iostream>
#include <arm_neon.h>
#include <stdio.h>

using namespace std;
using namespace Eigen;


int main(){
    /*Matrix3f coefficients;
    Vector3f results;
    Vector3f unknowns;

    coefficients << 1, 1, 1,  1, -1, 1,  1, 0, 0;
    results << 6, 2, 4;
    unknowns = coefficients.colPivHouseholderQr().solve(results);
    unknowns = (0.00001f < unknowns.array().abs()).select(unknowns, 0.0f);
    std::cout << unknowns;*/
    
    return 0;
}