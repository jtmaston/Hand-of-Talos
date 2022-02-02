#pragma once

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <stdint.h>
#include <iostream>
#include <arm_neon.h>

struct axis
{
    uint16_t x;
    uint16_t y;
};

class Joystick
{   
    public:
        Joystick();
        void Update_controllers();
        
    private:
        int Joystick::read_event(struct js_event *event);
        axis axes[2];
        js_event event;
        uint16_t device;
        void get_axis_state(js_event* event);
        
};