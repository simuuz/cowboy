#include "mem.h"

namespace natsukashii::core
{
MBC2::MBC2(std::vector<byte>& rom) : rom(rom)
{
  std::fill(ram.begin(), ram.end(), 0);
}

byte MBC2::Read(half addr)
{
  switch (addr)
  {
  case 0 ... 0x3fff:
    return rom[addr];
  case 0x4000 ... 0x7fff:
    return rom[0x4000 * romBank + (addr - 0x4000)];
  case 0xa000 ... 0xbfff:
    return ramEnable ? (0xf0 | (ram[addr & 0x1ff] & 0xf)) : 0xff;
  }
}

void MBC2::Write(half addr, byte val)
{
  switch(addr)
  {
  case 0 ... 0x3fff:
    if(bit<half, 8>(addr))
    {
      romBank = val;
      romBank = (romBank == 0) ? 1 : romBank;
    }
    else
    {
      ramEnable = (val & 0xf) == 0xa;
    }
    break;
  case 0xa000 ... 0xbfff:
    if(ramEnable)
    {
      ram[addr & 0x1ff] = val & 0xf;
    }
    break;
  }
}
} // natsukashii::core