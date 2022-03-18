#pragma once
#include <stdint.h>
#include <vector>

struct Instruction{
    uint8_t opcode;
    uint16_t params[10];
};

