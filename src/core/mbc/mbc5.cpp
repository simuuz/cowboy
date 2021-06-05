#include "mem.h"

namespace natsukashii::core
{
MBC5::MBC5(std::vector<byte>& rom) : rom(rom)
{
  std::fill(ram.begin(), ram.end(), 0);
}

byte MBC5::Read(half addr)
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

void MBC5::Write(half addr, byte val)
{
  switch (addr)
  {
  case 0 ... 0x1fff:
    ramEnable = ((val & 0xf) == 0x0a);
    break;
  case 0x2000 ... 0x2fff:
    romBank = val & 0xff;
    break;
  case 0x3000 ... 0x3fff:
    setbit<half, 8>(romBank, val & 1);
    break;
  case 0x4000 ... 0x5fff:
    ramBank = val & 0xf;
    break;
  case 0xa000 ... 0xbfff:
    if(ramEnable)
    {
      ram[0x2000 * ramBank + (addr - 0xa000)] = val;
    }
    break;
  default:
    printf("Unhandled write MBC5, addr %04X val %02X\n", addr, val);
  }
}

void MBC5::Save(std::string filename, std::string title)
{
  FILE* file = fopen(filename.c_str(), "wb");
  fwrite(title.data(), 1, sizeof(title.data()), file);
  fclose(file);
  file = fopen(filename.c_str(), "ab");
  fwrite(ram.data(), 1, sizeof(ram.data()), file);
  fclose(file);
}
} // natsukashii::core