#pragma once
#include "RobotArm.hpp"
#include <float.h>
#include <vector>
#include <QtConcurrent>

#include <chrono>
using namespace std::chrono;
 



class Watchdog
{
    public:
    std::vector<float32_t> destination;
    std::vector<float32_t> source;

    float32_t* state;
    uint16_t* time_mod;
    bool wd_active = false;

    RobotArm* dev;
    Watchdog(RobotArm* device, uint16_t* tmemd);
    QFuture<void> wd_thread;

    std::chrono::_V2::high_resolution_clock::time_point started;

    void register_wd();
    void deregister_wd();
    void handler();

};