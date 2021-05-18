#include "mem.h"
#include <memory.h>

namespace natsukashii::core
{
Mem::Mem(bool skip) : skip(skip)
{
  rom_opened = false;
  if (skip)
  {
    io.bootrom = 1;
    io.tac = 0;
    io.tima = 0;
    io.tma = 0;
    io.intf = 0;
    io.div = 0;
  }
  else
  {
    io.bootrom = 0;
    io.tac = 0;
    io.tima = 0;
    io.tma = 0;
    io.intf = 0;
    io.div = 0;
  }

  memset(bootrom, 0, BOOTROM_SZ);
  memset(extram, 0, EXTRAM_SZ);
  memset(eram, 0, ERAM_SZ);
  memset(wram, 0, WRAM_SZ);
  memset(hram, 0, HRAM_SZ);
}

void Mem::Reset()
{
  rom_opened = false;
  if (skip)
  {
    io.bootrom = 1;
    io.tac = 0;
    io.tima = 0;
    io.tma = 0;
    io.intf = 0;
    io.div = 0;
  }
  else
  {
    io.bootrom = 0;
    io.tac = 0;
    io.tima = 0;
    io.tma = 0;
    io.intf = 0;
    io.div = 0;
  }

  memset(extram, 0, EXTRAM_SZ);
  memset(eram, 0, ERAM_SZ);
  memset(wram, 0, WRAM_SZ);
  memset(hram, 0, HRAM_SZ);
}

void Mem::LoadROM(std::string path)
{
  rom.clear();
  std::ifstream file{path, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if (!file.is_open())
  {
    printf("Couldn't open %s\n", path.c_str());
    exit(1);
  }

  rom.insert(rom.begin(), std::istream_iterator<byte>(file), std::istream_iterator<byte>());
  file.close();

  rom_opened = true;
  printf("%s\n", mbcs[rom[0x147]].c_str());
  switch(rom[0x147])
  {
  case 0:
    cart = new NoMBC(rom);
    break;
  case 1 ... 3:
    cart = new MBC1(rom);
    break;
  case 5: case 6:
    cart = new MBC2(rom);
    break;
  case 0xF ... 0x13:
    cart = new MBC3(rom);
    break;
  case 0x19 ... 0x1E:
    cart = new MBC5(rom);
    break;
  }
}

bool Mem::LoadBootrom(std::string path)
{
  std::ifstream file{path, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if (!file.is_open())
  {
    printf("Couldn't open %s\n", path.c_str());
    return false;
  }

  file.read((char*)bootrom, BOOTROM_SZ);
  file.close();
  return true;
}

byte Mem::Read(half addr)
{
  switch (addr)
  {
  case 0 ... 0xff:
    if (io.bootrom == 0)
    {
      return bootrom[addr];
    }
    else
    {
      return cart->Read(addr);
    }
    break;
  case 0x100 ... 0x7fff:
    return cart->Read(addr);
  case 0xa000 ... 0xbfff:
    return cart->Read(addr);
  case 0xc000 ... 0xdfff:
    return wram[addr & 0x1fff];
  case 0xe000 ... 0xfdff:
    return eram[addr & 0x1dff];
  case 0xfea0 ... 0xfeff:
    return 0xff;
  case 0xff00 ... 0xff7f:
    return ReadIO(addr);
  case 0xff80 ... 0xfffe:
    return hram[addr & 0x7f];
  case 0xffff:
    return ie;
  }
}

void Mem::Write(half addr, byte val)
{
  switch (addr)
  {
  case 0 ... 0x7fff:
    cart->Write(addr, val);
    break;
  case 0xa000 ... 0xbfff:
    cart->Write(addr, val);
    break;
  case 0xc000 ... 0xdfff:
    wram[addr & 0x1fff] = val;
    break;
  case 0xe000 ... 0xfdff:
    eram[addr & 0x1dff] = val;
    break;
  case 0xfea0 ... 0xfeff:
    break;
  case 0xff00 ... 0xff7f:
    WriteIO(addr, val);
    break;
  case 0xff80 ... 0xfffe:
    hram[addr & 0x7f] = val;
    break;
  case 0xffff:
    ie = val;
    break;
  }
}

byte Mem::ReadIO(half addr)
{
  switch (addr & 0xff)
  {
  case 0x00:
    return 0xff;
  case 0x04:
    return io.div;
  case 0x05:
    return io.tima;
  case 0x06:
    return io.tma;
  case 0x07:
    return io.tac;
  case 0x0f:
    return io.intf;
  case 0x10 ... 0x1e:
    return 0xff;
  case 0x20 ... 0x26:
    return 0xff;
  case 0x4d:
    return 0xff;
  case 0x50:
    return io.bootrom;
  default:
    printf("IO READ: Unsupported IO %04X\n", addr);
    exit(1);
  }
}

void Mem::WriteIO(half addr, byte val)
{
  switch (addr & 0xff)
  {
  case 0x00:
    break;
  case 0x01:
    printf("%c", val);
    break;
  case 0x02:
    break;
  case 0x04:
    io.div = 0;
    break;
  case 0x05:
    io.tima = val;
    break;
  case 0x06:
    io.tma = val;
    break;
  case 0x0f:
    io.intf = val;
    break;
  case 0x07:
    io.tac = val;
    break;
  case 0x10 ... 0x1e:
    break;
  case 0x20 ... 0x26:
    break;  // STUB
  case 0x50:
    io.bootrom = val;
    break;
  case 0x7f:
    break;
  default:
    printf("IO WRITE: Unsupported IO %02x\n", addr & 0xff);
    exit(1);
  }
}
}  // namespace natsukashii::core