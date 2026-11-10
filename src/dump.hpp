// dump.hpp — два способи подивитися на байт.
// Надано.
#pragma once

#include "memory.hpp"

// Вивести всю коробку, по 16 байт на рядок: адреса, hex, ASCII-стовпець.
void dump(const Memory& mem);

// Вивести ОДИН байт чотирма способами: десятковий, hex, двійковий, символ.
// Приклад для байта 65:   65  0x41  0b01000001  'A'
// TODO(lab-01, M3): реалізувати в dump.cpp.
void show_byte(Byte b);
