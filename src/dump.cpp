// dump.cpp — hex + ASCII, як у Unix-інструмента `hexdump -C`.
//
// Функція `dump` нижче вже надана і працює, окрім одного TODO.
// Прочитайте її перед змінами: це форма майже кожного циклу, який ви
// писатимете цього семестру (зовнішній цикл по рядках, внутрішній по стовпцях).
//
// `show_byte` внизу — ВАША РОБОТА.
#include "dump.hpp"

#include <iomanip>
#include <iostream>

// Скільки байтів ми друкуємо в одному рядку. Спробуйте змінити на 8 і перебудувати.
const std::size_t BYTES_PER_LINE = 16;

// Чи вміє термінал друкувати цей байт як символ?
// 0x20 — пробіл, 0x7E — '~'. Все поза цим діапазоном ми виводимо як '.'.
static bool is_printable(Byte b) { return b >= 0x20 && b <= 0x7E; }

void dump(const Memory& mem) {
    // std::hex переводить потік у шістнадцятковий формат; setfill/setw доповнює нулями,
    // щоб усі числа мали однакову ширину й стовпці вирівнювалися.
    for (std::size_t row = 0; row < MEM_SIZE; row += BYTES_PER_LINE) {

        // Стовпець адрес: 0000, 0010, 0020, ...
        std::cout << std::hex << std::setfill('0') << std::setw(4) << row << "  ";

        // Стовпець hex: 16 байт, по два символи кожен.
        for (std::size_t col = 0; col < BYTES_PER_LINE; ++col) {
            Byte b = mem.data[row + col];
            std::cout << std::setw(2) << static_cast<int>(b) << ' ';
        }

        std::cout << " |";

        // ASCII-колонка.
        for (std::size_t col = 0; col < BYTES_PER_LINE; ++col) {
            Byte b = mem.data[row + col];
            if (is_printable(b)) {
                std::cout << static_cast<char>(b);
            } else {
                std::cout << '.';
            }
        }

        std::cout << "|\n";
    }

    // Повертаємо потік до десяткової системи, інакше всі наступні числа будуть hex.
    std::cout << std::dec << std::setfill(' ');
}

void show_byte(Byte b) {
    std::cout << static_cast<int>(b) << "  0x";

    std::cout << std::hex << static_cast<int>(b);
    std::cout << std::dec << "  0b";

    for (int bit = 7; bit >= 0; --bit) {
        std::cout << ((b >> bit) & 1u);
    }

    std::cout << "  '";
    if (is_printable(b)) {
        std::cout << static_cast<char>(b);
    } else {
        std::cout << '.';
    }
    std::cout << "'\n";
}
