#include "alu.hpp"

namespace {
AluResult make_result(Byte value) {
    AluResult r;
    r.value = value;
    r.z = (value == 0);
    r.n = ((value & 0x80u) != 0u);
    r.c = false;
    return r;
}
}

AluResult alu_add(Byte a, Byte b) {
    const std::uint16_t total = static_cast<std::uint16_t>(a) + static_cast<std::uint16_t>(b);
    AluResult r = make_result(static_cast<Byte>(total));
    r.c = (total > 0xFFu);
    return r;
}

AluResult alu_sub(Byte a, Byte b) {
    const std::uint16_t total = static_cast<std::uint16_t>(a) - static_cast<std::uint16_t>(b);
    AluResult r = make_result(static_cast<Byte>(total));
    r.c = (a < b);
    return r;
}

AluResult alu_and(Byte a, Byte b) {
    AluResult r = make_result(static_cast<Byte>(a & b));
    return r;
}

AluResult alu_or(Byte a, Byte b) {
    AluResult r = make_result(static_cast<Byte>(a | b));
    return r;
}

AluResult alu_xor(Byte a, Byte b) {
    AluResult r = make_result(static_cast<Byte>(a ^ b));
    return r;
}

AluResult alu_not(Byte a) {
    AluResult r = make_result(static_cast<Byte>(~a));
    return r;
}

AluResult alu_shl(Byte a) {
    const bool carry = ((a & 0x80u) != 0u);
    AluResult r = make_result(static_cast<Byte>(a << 1u));
    r.c = carry;
    return r;
}

AluResult alu_shr(Byte a) {
    const bool carry = ((a & 0x01u) != 0u);
    AluResult r = make_result(static_cast<Byte>(a >> 1u));
    r.c = carry;
    return r;
}

AluResult alu_inc(Byte a) {
    AluResult r = alu_add(a, 1);
    return r;
}

AluResult alu_dec(Byte a) {
    AluResult r = alu_sub(a, 1);
    return r;
}
