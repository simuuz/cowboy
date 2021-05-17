#pragma once
#include <array>
#include <iterator>
#include <vector>
#include "common.h"

constexpr int BOOTROM_SZ = 0x100;
constexpr int EXTRAM_SZ = 0x2000;
constexpr int ERAM_SZ = 0x1e00;
constexpr int WRAM_SZ = 0x2000;
constexpr int HRAM_SZ = 0x7f;
constexpr int ROM_SZ_MIN = 0x8000;

namespace natsukashii::core
{
class Cartridge
{
public:
  virtual ~Cartridge() = default;
  virtual byte Read(half addr) = 0;
  virtual void Write(half addr, byte val) = 0;

  word GetRAMSize(byte index)
  {
    word sizes[6] = { 0, 2048, 8192, 32768, 131072, 65536 };
    return sizes[index];
  }

  word GetROMSize(byte index)
  {
    return (32 * 1024) << (word)index;
  }
};

class ROMOnly : public Cartridge
{
public:
  ROMOnly(std::vector<byte>& rom) : rom(rom) {}

  byte Read(half addr) override
  {
    switch(addr)
    {
      case 0 ... 0x7FFF:
      return rom[addr];
      case 0xa000 ... 0xbfff:
      return 0xff;
    }
  }

  void Write(half addr, byte val) override
  {
    switch(addr)
    {
      case 0 ... 0x7FFF:
      printf("Tried to write to ROM, addr: %04X, val: %02X\n", addr, val);
      exit(1);
      case 0xa000 ... 0xbfff:
      break;
    }
  }
private:
  std::vector<byte> rom;
};

class Mem
{
public:
  Mem(bool skip);
  void LoadROM(std::string filename);
  bool LoadBootrom(std::string filename);
  void Reset();
  byte Read(half addr);
  void Write(half addr, byte val);

  byte ie = 0;
  bool skip;

  struct IO
  {
    byte bootrom = 1, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
  } io;

  friend class Ppu;
  bool rom_opened = false;

private:
  std::unique_ptr<Cartridge> mapper;
  Cartridge* cart;

  void WriteIO(half addr, byte val);
  byte ReadIO(half addr);

  byte bootrom[BOOTROM_SZ];
  byte extram[EXTRAM_SZ];
  byte wram[WRAM_SZ];
  byte eram[ERAM_SZ];
  byte hram[HRAM_SZ];
  std::vector<byte> rom;

  bool dpad = false;
  bool button = false;

  void HandleJoypad(byte val)
  {
    button = !bit<byte, 5>(val);
    dpad = !bit<byte, 4>(val);
  }

  byte GetJoypad();
};
}  // namespace natsukashii::core