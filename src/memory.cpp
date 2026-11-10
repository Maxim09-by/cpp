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
