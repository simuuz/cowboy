#include "mem.h"

namespace natsukashii::core
{
MBC3::MBC3(std::vector<byte>& rom) : rom(rom)
{
  std::fill(ram.begin(), ram.end(), 0);
}

byte MBC3::Read(half addr)
{
  switch (addr)
  {
  case 0 ... 0x3fff:
    return rom[addr];
  case 0x4000 ... 0x7fff:
    return rom[0x4000 * romBank + (addr - 0x4000)];
  case 0xa000 ... 0xbfff:
    return ramEnable ? ram[0x2000 * ramBank + (addr - 0xa000)] : 0xff;
  }
}

void MBC3::Write(half addr, byte val)
{
  switch (addr)
  {
  case 0 ... 0x1fff:
    ramEnable = ((val & 0xf) == 0x0a);
    break;
  case 0x2000 ... 0x3fff:
    romBank = val & 0x7f;
    romBank = (romBank == 0) ? 1 : romBank;
    break;
  case 0x4000 ... 0x5fff:
    if (val < 4)
    {
      ramBank = val;
    }
    break;
  case 0xa000 ... 0xbfff:
    if (ramEnable)
    {
      ram[0x2000 * ramBank + (addr - 0xA000)] = val;
    }
    break;
  }
}
} // natsukashii::core