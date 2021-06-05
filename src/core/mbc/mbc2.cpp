#include "mem.h"

namespace natsukashii::core
{
MBC2::MBC2(std::vector<byte>& rom, std::string savefile) : rom(rom)
{
  std::ifstream file{savefile, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if (!file.is_open())
  {
    ram.fill(0);
  }
  else
  {
    file.read((char*)ram.data(), ERAM_SZ);
    file.close();
  }
}

byte MBC2::Read(half addr)
{
  switch (addr)
  {
  case 0 ... 0x3fff:
    return rom[addr];
  case 0x4000 ... 0x7fff:
    return rom[(0x4000 * romBank + (addr - 0x4000)) % rom.size()];
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

void MBC2::Save(std::string filename)
{
  FILE* file = fopen(filename.c_str(), "wb");
  fwrite(ram.data(), 1, ERAM_SZ, file);
  fclose(file);
}
} // natsukashii::core