#include "mem.h"

namespace natsukashii::core
{
MBC3::MBC3(std::vector<byte>& rom, std::string savefile) : rom(rom)
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

byte MBC3::Read(half addr)
{
  switch (addr)
  {
  case 0 ... 0x3fff:
    return rom[addr];
  case 0x4000 ... 0x7fff:
    return rom[(0x4000 * romBank + (addr - 0x4000)) % rom.size()];
  case 0xa000 ... 0xbfff:
    return ramEnable ? ram[(0x2000 * ramBank + (addr - 0xa000)) % ERAM_SZ] : 0xff;
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
      ram[(0x2000 * ramBank + (addr - 0xA000)) % ERAM_SZ] = val;
    }
    break;
  }
}

void MBC3::Save(std::string filename)
{
  FILE* file = fopen(filename.c_str(), "wb");
  fwrite(ram.data(), 1, ERAM_SZ, file);
  fclose(file);
}
} // natsukashii::core