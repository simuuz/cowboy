#include "mem.h"
#include <memory.h>
#include <filesystem>

namespace natsukashii::core
{
Mem::~Mem()
{
  if(cart != nullptr)
    cart->Save(savefile);
}

void Mem::SaveState(std::ofstream& savestate) {
  if(cart != nullptr)
    savestate.write((char*)cart->GetRAM(), EXTRAM_SZ);
  
  savestate.write((char*)wram, WRAM_SZ);
  savestate.write((char*)hram, HRAM_SZ);
  savestate.write((char*)&ie, 1);
}

void Mem::LoadState(std::ifstream& loadstate) {
  if(cart != nullptr)
    cart->SetRam(loadstate);
  
  loadstate.read((char*)wram, WRAM_SZ);
  loadstate.read((char*)hram, HRAM_SZ);
  loadstate.read((char*)&ie, 1);
}

Mem::Mem(bool skip, std::string bootrom_path) : skip(skip)
{
  rom_opened = false;
  
  io.tac = 0;
  io.tima = 0;
  io.tma = 0;
  io.intf = 0;
  io.div = 0;
  io.joy.raw = 0xff;

  io.bootrom = skip ? 1 : 0;

  LoadBootROM(bootrom_path);
  memset(wram, 0, WRAM_SZ);
  memset(hram, 0, HRAM_SZ);
}

void Mem::Reset()
{
  if(cart != nullptr) {
    cart->Save(savefile);
  }

  io.tac = 0;
  io.tima = 0;
  io.tma = 0;
  io.intf = 0;
  io.div = 0;
  io.joy.raw = 0xff;

  io.bootrom = skip ? 1 : 0;

  memset(wram, 0, WRAM_SZ);
  memset(hram, 0, HRAM_SZ);
}

void Mem::LoadROM(std::string path)
{
  std::filesystem::path filename = path;
  savefile = filename.replace_extension("sav").string();
  if(cart != nullptr)
  {
    delete cart;
  }
  std::ifstream file{path, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if (!file.is_open())
  {
    printf("Couldn't open %s\n", path.c_str());
    exit(1);
  }

  rom.insert(rom.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());
  file.close();

  rom_opened = true;
  
  switch(rom[0x147])
  {
  case 0:
    cart = new NoMBC(rom);
    break;
  case 1 ... 3:
    cart = new MBC1(rom, savefile);
    break;
  case 5: case 6:
    cart = new MBC2(rom, savefile);
    break;
  case 0xF ... 0x13:
    cart = new MBC3(rom, savefile);
    break;
  case 0x19 ... 0x1E:
    cart = new MBC5(rom, savefile);
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

u8 Mem::Read(u16 addr)
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
  case 0xc000 ... 0xfdff:
    return wram[addr & 0x1fff];
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

void Mem::Write(u16 addr, u8 val)
{
  switch (addr)
  {
  case 0 ... 0x7fff:
    cart->Write(addr, val);
    break;
  case 0xa000 ... 0xbfff:
    cart->Write(addr, val);
    break;
  case 0xc000 ... 0xfdff:
    wram[addr & 0x1fff] = val;
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
  default: break;
  }
}

u8 Mem::ReadIO(u16 addr)
{
  switch (addr & 0xff)
  {
  case 0:
    return io.joy.raw;
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
  case 0x50:
    return io.bootrom;
  default:
    return 0xff;
  }
}

void Mem::WriteIO(u16 addr, u8 val)
{
  switch (addr & 0xff)
  {
  case 0:
    HandleJoypad(val);
    break;
  case 0x01: case 0x02: break;
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
  case 0x50:
    io.bootrom = val;
    break;
  default:
    break;
  }
}

void Mem::HandleJoypad(u8 val)
{
  button = !bit<u8, 5>(val);
  dpad = !bit<u8, 4>(val);
}

void Mem::DoInputs(int key)
{
  u8 input = ((u8)(!button) << 5) | ((u8)(!dpad) << 4);
  u8 cond = (button << 1) | dpad;

  switch(cond) {
  case 0b00:
    input |= 0xff;
    break;
  case 0b01:
    input |= (!(key == GLFW_KEY_DOWN ) << 3) |
             (!(key == GLFW_KEY_UP   ) << 2) |
             (!(key == GLFW_KEY_LEFT ) << 1) |
             (!(key == GLFW_KEY_RIGHT) << 0);
    break;
  case 0b10:
    input |= (!(key == GLFW_KEY_ENTER      ) << 3) |
             (!(key == GLFW_KEY_RIGHT_SHIFT) << 2) |
             (!(key == GLFW_KEY_Z          ) << 1) |
             (!(key == GLFW_KEY_X          ) << 0);
    break;
  case 0b11:
    input |= (!((key == GLFW_KEY_DOWN  || key == GLFW_KEY_ENTER      )) << 3) |
             (!((key == GLFW_KEY_UP    || key == GLFW_KEY_RIGHT_SHIFT)) << 2) |
             (!((key == GLFW_KEY_LEFT  || key == GLFW_KEY_Z          )) << 1) |
             (!((key == GLFW_KEY_RIGHT || key == GLFW_KEY_X          )) << 0);
    break;
  }

  io.joy.write(input);
}
}  // namespace natsukashii::core