g++ -c Arm_lib.cpp
ar rvs libarm.a Arm_lib.o
g++ main.cpp libarm.a -li2c
