#include "mem.h"
#include <memory.h>

Mem::Mem(bool skip, std::string path1, std::string path2) : skip(skip)
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
  load_rom(path1);
  load_bootrom(path2);
}

void Mem::reset()
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

void Mem::load_rom(std::string path)
{
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
  cart.loadROM(rom);
}

bool Mem::load_bootrom(std::string path)
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

byte Mem::read(half addr)
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
      return cart.readROM(addr);
    }
    break;
  case 0x100 ... 0x7fff:
    return cart.readROM(addr);
  case 0xa000 ... 0xbfff:
    return cart.readERAM(addr);
  case 0xc000 ... 0xdfff:
    return wram[addr & 0x1fff];
  case 0xe000 ... 0xfdff:
    return eram[addr & 0x1dff];
  case 0xfea0 ... 0xfeff:
    return 0xff;
  case 0xff00 ... 0xff7f:
    return read_io(addr);
  case 0xff80 ... 0xfffe:
    return hram[addr & 0x7f];
  case 0xffff:
    return ie;
  }
}

void Mem::write(half addr, byte val)
{
  switch (addr)
  {
  case 0 ... 0x7fff:
    cart.writeROM(addr, val);
    break;
  case 0xa000 ... 0xbfff:
    cart.writeERAM(addr, val);
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
    write_io(addr, val);
    break;
  case 0xff80 ... 0xfffe:
    hram[addr & 0x7f] = val;
    break;
  case 0xffff:
    ie = val;
    break;
  }
}

byte Mem::read_io(half addr)
{
  switch (addr & 0xff)
  {
  case 0x00:
    return get_joypad();
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

void Mem::write_io(half addr, byte val)
{
  switch (addr & 0xff)
  {
  case 0x00:
    handle_joypad(val);
    break;
  case 0x01:
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

byte Mem::get_joypad()
{
  byte num = 0xff;
  setbit<byte, 5>(num, !button);
  setbit<byte, 4>(num, !button);

  switch (evt.type)
  {
  case SDL_KEYDOWN:
    switch (evt.key.keysym.sym)
    {
    case SDLK_RETURN:
      if (button)
      {
        setbit<byte, 3>(num, 0);
      }
      break;
    case SDLK_d:
      if (button)
      {
        setbit<byte, 2>(num, 0);
      }
      break;
    case SDLK_a:
      if (button)
      {
        setbit<byte, 1>(num, 0);
      }
      break;
    case SDLK_s:
      if (button)
      {
        setbit<byte, 0>(num, 0);
      }
      break;
    case SDLK_DOWN:
      if (dpad)
      {
        setbit<byte, 3>(num, 0);
      }
      break;
    case SDLK_UP:
      if (dpad)
      {
        setbit<byte, 2>(num, 0);
      }
      break;
    case SDLK_LEFT:
      if (dpad)
      {
        setbit<byte, 1>(num, 0);
      }
      break;
    case SDLK_RIGHT:
      if (dpad)
      {
        setbit<byte, 0>(num, 0);
      }
      break;
    }
    break;
  }

  return num;
}