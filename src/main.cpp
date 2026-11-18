// main.cpp — інтерфейс користувача.
//
// Надано повністю. Цей файл є каркасом: він читає рядок, розбиває його на
// слова і викликає одну з ваших функцій. Тут використовуються `while`, `if` та
// функції, які в курсі пояснюють лише в Лабораторних 4 і 7. Це зроблено
// навмисно — на Lab 1 ви читаєте цей файл, але не пишете його.
//
// Те, що ви додаєте в наступних лабораторних, — це ще один `else if` для
// кожної команди.
#include <iostream>
#include <sstream>
#include <string>

#include "alu.hpp"
#include "cpu.hpp"
#include "dump.hpp"
#include "memory.hpp"

// Перетворює слово на число. Приймає десятковий формат (65) і hex (0x41).
// Повертає false, якщо слово взагалі не є числом.
static bool parse_number(const std::string& word, long& out) {
    try {
        std::size_t used = 0;
        // base 0 означає: дивитися на префікс. "0x41" — це hex, "65" — десяткове число.
        out = std::stol(word, &used, 0);
        return used == word.size();  // відхиляє типу "12abc"
    } catch (...) {
        return false;
    }
}

static void print_help() {
    std::cout << "команди:\n"
              << "  dump              вивести всі " << MEM_SIZE << " байтів\n"
              << "  get <addr>        показати один байт у 4 формах\n"
              << "  set <addr> <val>  записати один байт (dec або 0x hex)\n"
              << "  reg <a|b|h|pc> <val> встановити регістр CPU\n"
              << "  regs              показати регістри CPU та флаги\n"
              << "  step              виконати одну інструкцію\n"
              << "  run               виконувати до HALT або помилки\n"
              << "  find <lo> <hi> <byte>  знайти перше входження байта в пам'яті\n"
              << "  alu <op> <a> [b]  виконати ALU-операцію над байтом або двома байтами\n"
              << "  help              показати цей список\n"
              << "  quit              вийти\n";
}

static void print_alu_result(const std::string& op, Byte lhs, Byte rhs) {
    AluResult result;
    if (op == "add") {
        result = alu_add(lhs, rhs);
    } else if (op == "sub") {
        result = alu_sub(lhs, rhs);
    } else if (op == "and") {
        result = alu_and(lhs, rhs);
    } else if (op == "or") {
        result = alu_or(lhs, rhs);
    } else if (op == "xor") {
        result = alu_xor(lhs, rhs);
    } else if (op == "not") {
        result = alu_not(lhs);
        rhs = 0;
    } else if (op == "shl") {
        result = alu_shl(lhs);
        rhs = 0;
    } else if (op == "shr") {
        result = alu_shr(lhs);
        rhs = 0;
    } else {
        std::cout << "використання: alu <add|sub|and|or|xor|not|shl|shr> <a> [b]\n";
        return;
    }

    std::cout << "result=" << static_cast<int>(result.value)
              << " z=" << (result.z ? 1 : 0)
              << " n=" << (result.n ? 1 : 0)
              << " c=" << (result.c ? 1 : 0)
              << '\n';
}

int main() {
    Memory mem;  // 4096 bytes, on the stack, zeroed by the {} in memory.hpp
    CPU cpu;
    cpu.mem = &mem;

    std::cout << "ember 0.1 - 4096 bytes of memory you can see. Type `help`.\n";

    std::string line;
    while (true) {
        std::cout << "ember> ";

        // getline reads one whole line. It returns false at end of input
        // (Ctrl-D), which is how the loop ends if you never type `quit`.
        if (!std::getline(std::cin, line)) {
            std::cout << '\n';
            break;
        }

        // Split the line into words: "set 0 65" -> cmd="set", args "0" and "65".
        std::istringstream words(line);
        std::string cmd;
        words >> cmd;

        if (cmd.empty()) {
            continue;  // the user just pressed Enter
        } else if (cmd == "quit" || cmd == "exit") {
            break;
        } else if (cmd == "help") {
            print_help();
        } else if (cmd == "dump") {
            dump(mem);
        } else if (cmd == "get") {
            std::string a;
            long addr = 0;
            if (!(words >> a) || !parse_number(a, addr)) {
                std::cout << "usage: get <addr>\n";
            } else if (addr < 0) {
                std::cout << "address must not be negative\n";
            } else {
                show_byte(mem_get(mem, static_cast<std::size_t>(addr)));
            }
        } else if (cmd == "set") {
            std::string a, v;
            long addr = 0, value = 0;
            if (!(words >> a) || !(words >> v) || !parse_number(a, addr) ||
                !parse_number(v, value)) {
                std::cout << "usage: set <addr> <value>\n";
            } else if (addr < 0) {
                std::cout << "address must not be negative\n";
            } else if (value < 0 || value > 255) {
                // A cell holds ONE byte. 256 does not fit. Lab 1, theory 3.
                std::cout << "a byte is 0..255, got " << value << '\n';
            } else if (!mem_set(mem, static_cast<std::size_t>(addr),
                                static_cast<Byte>(value))) {
                std::cout << "address " << addr << " is outside 0.." << MEM_SIZE - 1
                          << '\n';
            }
        } else if (cmd == "reg") {
            std::string r, v;
            long value = 0;
            if (!(words >> r) || !(words >> v) || !parse_number(v, value) || value < 0) {
                std::cout << "usage: reg <a|b|h|pc> <value>\n";
            } else if (r == "a" || r == "b") {
                if (value > 255) {
                    std::cout << "A/B must be 0..255\n";
                } else if (r == "a") {
                    cpu.a = static_cast<Byte>(value);
                } else {
                    cpu.b = static_cast<Byte>(value);
                }
            } else if (r == "h") {
                if (value > 65535) std::cout << "H must be 0..65535\n";
                else cpu.h = static_cast<std::uint16_t>(value);
            } else if (r == "pc") {
                if (value > 65535) std::cout << "PC must be 0..65535\n";
                else cpu.pc = static_cast<std::uint16_t>(value);
            } else {
                std::cout << "unknown register: " << r << '\n';
            }
        } else if (cmd == "regs") {
            dump_regs(cpu);
        } else if (cmd == "step") {
            step(cpu);
            dump_regs(cpu);
        } else if (cmd == "run") {
            const std::size_t STEP_LIMIT = 100000;
            std::size_t steps = 0;
            for (; steps < STEP_LIMIT && !cpu.halted; ++steps) {
                step(cpu);
            }
            if (!cpu.halted) {
                std::cout << "step limit reached: " << STEP_LIMIT << '\n';
            }
            dump_regs(cpu);
        } else if (cmd == "find") {
            std::string lo_word, hi_word, byte_word;
            long lo = 0, hi = 0, needle = 0;
            if (!(words >> lo_word) || !(words >> hi_word) || !(words >> byte_word) ||
                !parse_number(lo_word, lo) || !parse_number(hi_word, hi) ||
                !parse_number(byte_word, needle)) {
                std::cout << "usage: find <lo> <hi> <byte>\n";
            } else if (lo < 0 || hi < 0 || lo > static_cast<long>(MEM_SIZE) || hi > static_cast<long>(MEM_SIZE) ||
                       needle < 0 || needle > 255) {
                std::cout << "find: lo/hi must be 0.." << MEM_SIZE << ", byte 0..255\n";
            } else {
                const std::uint16_t found = mem_find(mem,
                    static_cast<std::uint16_t>(lo), static_cast<std::uint16_t>(hi),
                    static_cast<Byte>(needle));
                if (found == hi) std::cout << "not found\n";
                else std::cout << "found=" << found << '\n';
            }
        } else if (cmd == "alu") {
            std::string op, a, b;
            long left = 0, right = 0;
            if (!(words >> op) || !(words >> a) || !parse_number(a, left) || left < 0 || left > 255) {
                std::cout << "usage: alu <add|sub|and|or|xor|not|shl|shr> <a> [b]\n";
            } else if (op == "not" || op == "shl" || op == "shr") {
                print_alu_result(op, static_cast<Byte>(left), 0);
            } else if (words >> b && parse_number(b, right) && right >= 0 && right <= 255) {
                print_alu_result(op, static_cast<Byte>(left), static_cast<Byte>(right));
            } else {
                std::cout << "usage: alu <add|sub|and|or|xor> <a> <b>; unary: <not|shl|shr> <a>\n";
            }
        } else {
            std::cout << "unknown command: " << cmd << " (try `help`)\n";
        }
    }

    return 0;  // 0 means "success" to the operating system
}
