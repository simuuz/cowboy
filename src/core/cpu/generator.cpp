#include "cpu.h"

namespace natsukashii::core
{
template <byte instruction>
static constexpr auto Cpu::NoPrefixGenerator() -> Cpu::Handler
{
  if constexpr (instruction == 0xD3 || instruction == 0xDB || instruction == 0xDD ||
                instruction == 0xE3 || instruction == 0xE4 || instruction == 0xEB ||
                instruction == 0xEC || instruction == 0xED || instruction == 0xF4 ||
                instruction == 0xFC || instruction == 0xFD) {
    return invalid;
  } else if constexpr (instruction == 0x00 || instruction == 0x10 || instruction == 0x77 ||
                instruction == 0xF3 || instruction == 0xFB || instruction == 0xCB) {
    return misc;
  } else if constexpr (((instruction >= 0x40 && instruction <= 0x75) || (instruction >= 0x77 && instruction <= 0x7F)) ||
                         instruction == 0x06 || instruction == 0x16  ||  instruction == 0x26 || instruction == 0x36 ||
                         instruction == 0x0E || instruction == 0x1E  ||  instruction == 0x2E || instruction == 0x3E ||
                         instruction == 0xE0 || instruction == 0xF0  ||  instruction == 0xE2 || instruction == 0xF2 ||
                         instruction == 0x02 || instruction == 0x12  ||  instruction == 0x22 || instruction == 0x32 ||
                         instruction == 0x0a || instruction == 0x1a  ||  instruction == 0x2a || instruction == 0x3a ||
                         instruction == 0xea || instruction == 0xfa) {
    return load8;
  } else if constexpr (instruction == 0x01 || instruction == 0x11 || instruction == 0x21 || instruction == 0x31 ||
                       instruction == 0x08 || instruction == 0xf9 || instruction == 0xc1 || instruction == 0xd1 ||
                       instruction == 0xe1 || instruction == 0xf1 || instruction == 0xc5 || instruction == 0xd5 ||
                       instruction == 0xe5 || instruction == 0xf5) {
    return load16;
  } else if constexpr (instruction == 0x04 || instruction == 0x14 || instruction == 0x24 || instruction == 0x34 ||
                       instruction == 0x0c || instruction == 0x1c || instruction == 0x2c || instruction == 0x3c || 
                       instruction == 0x05 || instruction == 0x15 || instruction == 0x25 || instruction == 0x35 ||
                       instruction == 0x0d || instruction == 0x1d || instruction == 0x2d || instruction == 0x3d ||
                       instruction == 0x27 || instruction == 0x2F || instruction == 0x37 || instruction == 0x3f || 
                       ((instruction >= 0x80 && instruction <= 0x87) || instruction == 0xC6) || 
                       ((instruction >= 0x88 && instruction <= 0x8F) || instruction == 0xCE) ||
                       ((instruction >= 0x90 && instruction <= 0x97) || instruction == 0xD6) ||
                       ((instruction >= 0x98 && instruction <= 0x9F) || instruction == 0xDE) ||
                       ((instruction >= 0xa0 && instruction <= 0xa7) || instruction == 0xE6) ||
                       ((instruction >= 0xA8 && instruction <= 0xAF) || instruction == 0xEE) ||
                       ((instruction >= 0xb0 && instruction <= 0xb7) || instruction == 0xF6) ||
                       ((instruction >= 0xb8 && instruction <= 0xbf) || instruction == 0xFE)) {
    return alu8;
  } else if constexpr (instruction == 0xE8 || instruction == 0xF8 || instruction == 0x03 || instruction == 0x13 ||
                       instruction == 0x23 || instruction == 0x33 || instruction == 0x0b || instruction == 0x1b ||
                       instruction == 0x2b || instruction == 0x3b || instruction == 0x09 || instruction == 0x19 ||
                       instruction == 0x29 || instruction == 0x39) {
    return alu16;
  } else if constexpr (instruction == 0x07 || instruction == 0x0F || instruction == 0x17 || instruction == 0x1F) {
    return bit8;
  }
}
} // natsukashii::core