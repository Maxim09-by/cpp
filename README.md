# ember — Lab 01: box of bytes

Це перша лабораторна. Тут ми працюємо з байтом, типами, пам'яттю і тим, як один і той самий набір бітів можна прочитати по-різному.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/ember
```

## `sizeof` on this machine

На цій машині отримані такі розміри типів:

| Type | `sizeof` (bytes) | Notes |
|---|---:|---|
| `char` | 1 | one byte |
| `bool` | 1 | boolean |
| `short` | 2 | 16-bit integer |
| `int` | 4 | usual signed int |
| `long` | 8 | 64-bit integer |
| `long long` | 8 | 64-bit integer |
| `float` | 4 | 32-bit floating point |
| `double` | 8 | 64-bit floating point |
| `void*` | 8 | pointer |
| `std::uint8_t` | 1 | exact 1 byte |
| `std::uint16_t` | 2 | exact 2 bytes |
| `std::int32_t` | 4 | exact 4 bytes |
| `std::uint32_t` | 4 | exact 4 bytes |
| `std::size_t` | 8 | size/count type |

## Terminal output used in the lab

```txt
ember> set 0 65
ember> set 1 66
ember> get 0
65  0x41  0b01000001  'A'
ember> dump
0000  41 42 00 00 00 00 00 00 00 00 00 00 00 00 00 00  |AB..............|
```

This is the key result for M2/M3:
- byte `65` is the same memory as `0x41`
- in binary it is `0b01000001`
- as a printable character it is `'A'`
- `dump` shows the memory as a hex view + ASCII column

## What is implemented here

- `mem_get` reads a byte from `Memory` and rejects invalid addresses
- `mem_set` writes one byte and returns `false` if the address is out of range
- `dump` prints the whole 4096-byte memory block in rows of 16 bytes
- `show_byte` prints a byte four ways: decimal, hex, binary, character

## Notes

- `Memory` stores `Byte data[MEM_SIZE]{}` and is zero-initialized.
- Addresses are valid from `0` to `MEM_SIZE - 1`.
- `get` and `set` must not read or write outside the box.
- The project is built with `-Wall -Wextra -Werror` and ASan + UBSan in Debug mode.

## Lab checklist

- [x] project builds
- [x] `dump` works
- [x] `get` shows four views of the same byte
- [x] `set` validates address and value
- [x] README contains `sizeof` table and terminal output
- [ ] public GitHub repository and tag `lab-01` are created/pushed separately
