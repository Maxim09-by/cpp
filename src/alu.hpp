#pragma once

#include "memory.hpp"

struct AluResult {
    Byte value = 0;
    bool z = false;
    bool n = false;
    bool c = false;
};

AluResult alu_add(Byte a, Byte b);
AluResult alu_sub(Byte a, Byte b);
AluResult alu_and(Byte a, Byte b);
AluResult alu_or(Byte a, Byte b);
AluResult alu_xor(Byte a, Byte b);
AluResult alu_not(Byte a);
AluResult alu_shl(Byte a);
AluResult alu_shr(Byte a);
AluResult alu_inc(Byte a);
AluResult alu_dec(Byte a);
