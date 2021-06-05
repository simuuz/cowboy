#include "mem.h"

namespace natsukashii::core
{
MBC1::MBC1(std::vector<byte>& rom) : rom(rom)
{
  ram.fill(0);
  romSize = rom[0x148];
  ramSize = rom[0x149];
}

byte MBC1::Read(half addr)
{
  byte zeroBank = 0;
  byte highBank = 0;
  switch (addr)
  {
  case 0 ... 0x3fff:
    if (mode)
    {
      switch (romSize)
      {
      case 0 ... 4:
        zeroBank = 0;
        break;
      case 5:
        setbit<byte, 5>(zeroBank, ramBank & 1);
        break;
      case 6:
        setbit<byte, 5>(zeroBank, ramBank & 1);
        setbit<byte, 6>(zeroBank, ramBank >> 1);
        break;
      }
      return rom[0x4000 * zeroBank + addr];
    }
    else
    {
      return rom[addr];
    }
    break;
  case 0x4000 ... 0x7fff:
    highBank = romBank & bitmasks[romSize];
    switch (romSize)
    {
    case 5:
      setbit<byte, 5>(highBank, ramBank & 1);
      break;
    case 6:
      setbit<byte, 5>(highBank, ramBank & 1);
      setbit<byte, 6>(highBank, ramBank >> 1);
      break;
    }
    return rom[0x4000 * highBank + (addr - 0x4000)];
  case 0xa000 ... 0xbfff:
    if (ramEnable)
    {
      if (ramSize == 0x01 || ramSize == 0x02)
      {
        return ram[(addr - 0xa000) % RAM_SIZES[ramSize]];
      }
      else if (ramSize == 0x03)
      {
        if (mode)
        {
        	return ram[0x2000 * ramBank + (addr - 0xa000)];
        }
        else
        {
          return ram[addr - 0xa000];
        }
      }
    }
    else
    {
      return 0xff;
    }
    break;
  }
}

void MBC1::Write(half addr, byte val) 
{
  switch (addr)
  {
  case 0 ... 0x1fff:
    ramEnable = ((val & 0xf) == 0xa);
    break;
  case 0x2000 ... 0x3fff:
    romBank = val & bitmasks[romSize];
    romBank = (romBank == 0) ? 1 : romBank;
    break;
  case 0x4000 ... 0x5fff:
    ramBank = val & 3;
    break;
  case 0x6000 ... 0x7fff:
    mode = val & 1;
    break;
  case 0xa000 ... 0xbfff:
    if (ramEnable)
    {
      if (ramSize == 0x01 || ramSize == 0x02)
      {
        ram[(addr - 0xa000) % RAM_SIZES[ramSize]] = val;
      }
      else if (ramSize == 0x03)
      {
        if (mode)
        {
          ram[0x2000 * ramBank + (addr - 0xa000)] = val;
        }
        else
        {
          ram[addr - 0xa000] = val;
        }
      }
    }
    break;
  }
}

void MBC1::Save(std::string filename, std::string title)
{
  FILE* file = fopen(filename.c_str(), "wb");
  fwrite(title.data(), 1, sizeof(title.data()), file);
  fclose(file);
  file = fopen(filename.c_str(), "ab");
  fwrite(ram.data(), 1, sizeof(ram.data()), file);
  fclose(file);
}
} // natsukashii::core