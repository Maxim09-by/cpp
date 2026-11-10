#include "cpu.hpp"

#include <iostream>

#include "alu.hpp"

std::uint8_t decode_group(Byte op) {
    return static_cast<std::uint8_t>((op >> 4) & 0x0Fu);
}

std::uint8_t decode_subcode(Byte op) {
    return static_cast<std::uint8_t>(op & 0x0Fu);
}

namespace {
void apply_alu_result(CPU& cpu, const AluResult& r) {
    cpu.a = r.value;
    cpu.f.z = r.z;
    cpu.f.n = r.n;
    cpu.f.c = r.c;
}
}

void step(CPU& cpu) {
    if (cpu.halted) {
        std::cout << "halted\n";
        return;
    }

    if (cpu.mem == nullptr) {
        std::cout << "cpu has no memory\n";
        return;
    }

    const Byte op = mem_get(*cpu.mem, cpu.pc);
    const std::uint8_t group = decode_group(op);
    const std::uint8_t subcode = decode_subcode(op);

    switch (group) {
    case 0x0:
        switch (subcode) {
        case 0x0:
            cpu.halted = true;
            break;
        case 0x1:
            break;
        default:
            std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
            break;
        }
        cpu.pc += 1;
        break;

    case 0x1: {
        switch (subcode) {
        case 0x0:
            apply_alu_result(cpu, alu_add(cpu.a, cpu.b));
            break;
        case 0x1:
            apply_alu_result(cpu, alu_sub(cpu.a, cpu.b));
            break;
        case 0x2:
            apply_alu_result(cpu, alu_and(cpu.a, cpu.b));
            break;
        case 0x3:
            apply_alu_result(cpu, alu_or(cpu.a, cpu.b));
            break;
        case 0x4:
            apply_alu_result(cpu, alu_xor(cpu.a, cpu.b));
            break;
        case 0x5:
            apply_alu_result(cpu, alu_not(cpu.a));
            break;
        case 0x6:
            apply_alu_result(cpu, alu_shl(cpu.a));
            break;
        case 0x7:
            apply_alu_result(cpu, alu_shr(cpu.a));
            break;
        case 0x8:
            apply_alu_result(cpu, alu_inc(cpu.a));
            break;
        case 0x9:
            apply_alu_result(cpu, alu_dec(cpu.a));
            break;
        default:
            std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
            break;
        }
        cpu.pc += 1;
        break;
    }

    default:
        std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
        cpu.pc += 1;
        break;
    }
}

void dump_regs(const CPU& cpu) {
    std::cout << "pc=" << cpu.pc << " a=" << static_cast<int>(cpu.a)
              << " b=" << static_cast<int>(cpu.b)
              << " z=" << (cpu.f.z ? 1 : 0) << " n=" << (cpu.f.n ? 1 : 0)
              << " c=" << (cpu.f.c ? 1 : 0) << " halted=" << (cpu.halted ? 1 : 0)
              << '\n';
}
