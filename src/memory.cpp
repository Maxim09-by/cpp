// memory.cpp — ВАША РОБОТА (Lab 1, M3).
//
// Дві функції, по кілька рядків кожна. Обидві потребують рівно одного `if`:
// адреса вважається дійсною, якщо вона менша за MEM_SIZE. Адреси рахуються з 0,
// тому остання правильна адреса — MEM_SIZE - 1, а не MEM_SIZE. Цей off-by-one і є
// предметом всього Lab 3.
#include "memory.hpp"

Byte mem_get(const Memory& mem, std::size_t addr) {
    if (addr >= MEM_SIZE) {
        return 0;
    }
    return mem.data[addr];
}

bool mem_set(Memory& mem, std::size_t addr, Byte value) {
    if (addr >= MEM_SIZE) {
        return false;
    }
    mem.data[addr] = value;
    return true;
}

std::uint16_t mem_get16(const Memory& mem, std::size_t addr) {
    if (addr >= MEM_SIZE || addr + 1 >= MEM_SIZE) {
        return 0;
    }
    Byte lo = mem_get(mem, addr);
    Byte hi = mem_get(mem, addr + 1);
    return static_cast<std::uint16_t>(lo | (static_cast<std::uint16_t>(hi) << 8));
}

bool mem_set16(Memory& mem, std::size_t addr, std::uint16_t value) {
    if (addr >= MEM_SIZE || addr + 1 >= MEM_SIZE) {
        return false;
    }
    if (!mem_set(mem, addr, static_cast<Byte>(value & 0xFFu))) {
        return false;
    }
    return mem_set(mem, addr + 1, static_cast<Byte>((value >> 8) & 0xFFu));
}
