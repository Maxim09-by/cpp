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

bool pc_read(const CPU& cpu, std::size_t offset, Byte& out) {
    if (cpu.pc + offset >= MEM_SIZE) {
        return false;
    }
    out = mem_get(*cpu.mem, static_cast<std::size_t>(cpu.pc) + offset);
    return true;
}

bool checked_h_load(CPU& cpu, Byte& value) {
    if (cpu.h >= MEM_SIZE) {
        std::cout << "H address outside memory: " << cpu.h << '\n';
        return false;
    }
    value = mem_get(*cpu.mem, cpu.h);
    return true;
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

    Byte op;
    if (!pc_read(cpu, 0, op)) {
        std::cout << "PC outside memory: " << cpu.pc << '\n';
        cpu.halted = true;
        return;
    }

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
        case 0x2:
            std::cout << static_cast<char>(cpu.a);
            break;
        case 0x3:
            std::cout << static_cast<int>(cpu.a) << ' ';
            break;
        default:
            std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
            break;
        }
        cpu.pc += 1;
        break;

    case 0x1:
        switch (subcode) {
        case 0x0: apply_alu_result(cpu, alu_add(cpu.a, cpu.b)); break;
        case 0x1: apply_alu_result(cpu, alu_sub(cpu.a, cpu.b)); break;
        case 0x2: apply_alu_result(cpu, alu_and(cpu.a, cpu.b)); break;
        case 0x3: apply_alu_result(cpu, alu_or(cpu.a, cpu.b)); break;
        case 0x4: apply_alu_result(cpu, alu_xor(cpu.a, cpu.b)); break;
        case 0x5: apply_alu_result(cpu, alu_not(cpu.a)); break;
        case 0x6: apply_alu_result(cpu, alu_shl(cpu.a)); break;
        case 0x7: apply_alu_result(cpu, alu_shr(cpu.a)); break;
        case 0x8: apply_alu_result(cpu, alu_inc(cpu.a)); break;
        case 0x9: apply_alu_result(cpu, alu_dec(cpu.a)); break;
        default:
            std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
            break;
        }
        cpu.pc += 1;
        break;

    case 0x2:
        switch (subcode) {
        case 0x0: {
            Byte imm;
            if (!pc_read(cpu, 1, imm)) { std::cout << "PC outside memory\n"; return; }
            cpu.a = imm; cpu.pc += 2; break;
        }
        case 0x1: {
            Byte imm;
            if (!pc_read(cpu, 1, imm)) { std::cout << "PC outside memory\n"; return; }
            cpu.b = imm; cpu.pc += 2; break;
        }
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x8: {
            Byte lo, hi;
            if (!pc_read(cpu, 1, lo) || !pc_read(cpu, 2, hi)) {
                std::cout << "PC outside memory\n"; return;
            }
            std::uint16_t addr = static_cast<std::uint16_t>(lo | (static_cast<std::uint16_t>(hi) << 8));
            if (subcode == 0x8) {
                cpu.h = addr;
            } else if (subcode == 0x2 || subcode == 0x3) {
                if (addr >= MEM_SIZE) { std::cout << "address outside memory: " << addr << '\n'; return; }
                if (subcode == 0x2) cpu.a = mem_get(*cpu.mem, addr); else cpu.b = mem_get(*cpu.mem, addr);
            } else {
                if (addr >= MEM_SIZE) { std::cout << "address outside memory: " << addr << '\n'; return; }
                if (subcode == 0x4) mem_set(*cpu.mem, addr, cpu.a); else mem_set(*cpu.mem, addr, cpu.b);
            }
            cpu.pc += 3;
            break;
        }
        case 0x6: cpu.a = cpu.b; cpu.pc += 1; break;
        case 0x7: cpu.b = cpu.a; cpu.pc += 1; break;
        case 0x9: {
            Byte value;
            if (checked_h_load(cpu, value)) { cpu.a = value; cpu.pc += 1; }
            break;
        }
        case 0xA: {
            if (cpu.h >= MEM_SIZE) { std::cout << "H address outside memory: " << cpu.h << '\n'; break; }
            mem_set(*cpu.mem, cpu.h, cpu.a); cpu.pc += 1; break;
        }
        case 0xB:
            if (cpu.h >= MEM_SIZE) { std::cout << "H address outside memory: " << cpu.h << '\n'; }
            else ++cpu.h; cpu.pc += 1; break;
        case 0xC:
            if (cpu.h == 0) { std::cout << "H address underflow\n"; }
            else --cpu.h; cpu.pc += 1; break;
        default:
            std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
            cpu.pc += 1;
            break;
        }
        break;

    default:
        std::cout << "unknown opcode: 0x" << std::hex << static_cast<int>(op) << std::dec << '\n';
        cpu.pc += 1;
        break;
    }
}

void dump_regs(const CPU& cpu) {
    std::cout << "pc=" << cpu.pc << " h=" << cpu.h
              << " a=" << static_cast<int>(cpu.a)
              << " b=" << static_cast<int>(cpu.b)
              << " z=" << (cpu.f.z ? 1 : 0) << " n=" << (cpu.f.n ? 1 : 0)
              << " c=" << (cpu.f.c ? 1 : 0) << " halted=" << (cpu.halted ? 1 : 0)
              << '\n';
}
