#include "mem.h"
#include <memory.h>
#include <filesystem>

namespace natsukashii::core
{
Mem::~Mem()
{
  size_t lastindex = filename.find_last_of("."); 
  std::string rawname = filename.substr(0, lastindex); 
  rawname += ".sav";
  cart->Save(rawname);
}

Mem::Mem(bool skip, std::string bootrom_path) : skip(skip)
{
  rom_opened = false;
  
  io.tac = 0;
  io.tima = 0;
  io.tma = 0;
  io.intf = 0;
  io.div = 0;

  io.bootrom = skip ? 1 : 0;

  LoadBootROM(bootrom_path);
  memset(extram, 0, EXTRAM_SZ);
  memset(eram, 0, ERAM_SZ);
  memset(wram, 0, WRAM_SZ);
  memset(hram, 0, HRAM_SZ);
}

void Mem::Reset()
{
  io.tac = 0;
  io.tima = 0;
  io.tma = 0;
  io.intf = 0;
  io.div = 0;

  io.bootrom = skip ? 1 : 0;

  memset(extram, 0, EXTRAM_SZ);
  memset(eram, 0, ERAM_SZ);
  memset(wram, 0, WRAM_SZ);
  memset(hram, 0, HRAM_SZ);
}

void Mem::LoadROM(std::string path)
{
  filename = path;
  if(cart != nullptr)
  {
    cart = nullptr;
  }
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

void Mem::LoadBootROM(std::string path)
{
  std::ifstream file{path, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if (!file.is_open())
  {
    printf("Couldn't open %s\n", path.c_str());
    exit(1);
  }

  file.read((char*)bootrom, BOOTROM_SZ);
  file.close();
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
  case 0xff00 ... 0xff77:
    return ReadIO(addr);
  case 0xff80 ... 0xfffe:
    return hram[addr & 0x7f];
  case 0xffff:
    return ie;
  default:
    return 0xff;
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
  case 0xff00 ... 0xff77:
    WriteIO(addr, val);
    break;
  case 0xff80 ... 0xfffe:
    hram[addr & 0x7f] = val;
    break;
  case 0xffff:
    ie = val;
    break;
  default:
    break;
  }
}

byte Mem::ReadIO(half addr)
{
  switch (addr & 0xff)
  {
  case 0:
    return io.joy;
  case 1 ... 0x02:
  case 0x30 ... 0x3f:
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
    return 0xff;
  }
}

void Mem::WriteIO(half addr, byte val)
{
  switch (addr & 0xff)
  {
  case 0:
    HandleJoypad(val);
    break;
  case 1 ... 0x02:
  case 0x30 ... 0x3f:
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
  case 0x10 ... 0x1e: case 0x20 ... 0x26:
    break;
  case 0x50:
    io.bootrom = val;
    break;
  default:
    break;
  }
}

void Mem::HandleJoypad(byte val)
{
  button = !bit<byte, 5>(val);
  dpad = !bit<byte, 4>(val);
}

void Mem::DoInputs(int key, int action)
{
  setbit<byte, 5>(io.joy, !button);
  setbit<byte, 4>(io.joy, !dpad);

  if(action == GLFW_PRESS) {
    if(button) {
      setbit<byte, 3>(io.joy, !(key == GLFW_KEY_ENTER));
      setbit<byte, 2>(io.joy, !(key == GLFW_KEY_RIGHT_SHIFT));
      setbit<byte, 1>(io.joy, !(key == GLFW_KEY_Z));
      setbit<byte, 0>(io.joy, !(key == GLFW_KEY_X));
    }

    if(dpad) {
      setbit<byte, 3>(io.joy, !(key == GLFW_KEY_DOWN));
      setbit<byte, 2>(io.joy, !(key == GLFW_KEY_UP));
      setbit<byte, 1>(io.joy, !(key == GLFW_KEY_LEFT));
      setbit<byte, 0>(io.joy, !(key == GLFW_KEY_RIGHT));
    }
  }
  
  if(action == GLFW_RELEASE) {
    if(button) {
      setbit<byte, 3>(io.joy, key == GLFW_KEY_ENTER);
      setbit<byte, 2>(io.joy, key == GLFW_KEY_RIGHT_SHIFT);
      setbit<byte, 1>(io.joy, key == GLFW_KEY_Z);
      setbit<byte, 0>(io.joy, key == GLFW_KEY_X);
    }

    if(dpad) {
      setbit<byte, 3>(io.joy, key == GLFW_KEY_DOWN);
      setbit<byte, 2>(io.joy, key == GLFW_KEY_UP);
      setbit<byte, 1>(io.joy, key == GLFW_KEY_LEFT);
      setbit<byte, 0>(io.joy, key == GLFW_KEY_RIGHT);
    }
  }
}
}  // namespace natsukashii::core