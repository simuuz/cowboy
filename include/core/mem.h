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
class NoMBC
{
public:
  NoMBC() {}

  void LoadROM(std::vector<byte>& rom) { this->rom = rom; }

  byte ReadROM(half addr) { return rom[addr]; }

  void WriteROM(half addr, byte val)
  {
    printf("Tried to write to ROM, addr: %04X val: %02X\n", addr, val);
    // exit(1);
  }

  byte ReadERAM(half addr)
  {
    printf("Read ERAM not available, addr: %04X", addr);
    return 0xff;
  }

  void WriteERAM(half addr, byte val)
  {
    printf("Write ERAM not available, addr: %04X val: %02X\n", addr, val);
  }

private:
  std::vector<byte> rom;
};

class MBC1
{
public:
  MBC1() { std::fill(ram.begin(), ram.end(), 0); }

  void LoadROM(std::vector<byte>& rom)
  {
    this->rom = rom;
    romSize = rom[0x148];
    ramSize = rom[0x149];
  }

  byte ReadROM(half addr)
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
          setbit<byte, 5>(zeroBank, ramBank.raw & 1);
          break;
        case 6:
          setbit<byte, 5>(zeroBank, ramBank.raw & 1);
          setbit<byte, 6>(zeroBank, ramBank.raw >> 1);
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
      highBank = romBank.raw & bitmasks[romSize];
      switch (romSize)
      {
      case 5:
        setbit<byte, 5>(highBank, ramBank.raw & 1);
        break;
      case 6:
        setbit<byte, 5>(highBank, ramBank.raw & 1);
        setbit<byte, 6>(highBank, ramBank.raw >> 1);
        break;
      }
      return rom[0x4000 * highBank + (addr - 0x4000)];
      break;
    }
  }

  byte ReadERAM(half addr)
  {
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
          return ram[0x2000 * ramBank.raw + (addr - 0xa000)];
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
  }

  void WriteROM(half addr, byte val)
  {
    switch (addr)
    {
    case 0 ... 0x1fff:
      ramEnable = ((val & 0xf) == 0xa);
      break;
    case 0x2000 ... 0x3fff:
      romBank.raw = val & bitmasks[romSize];
      romBank.raw = (romBank.raw == 0) ? 1 : romBank.raw;
      break;
    case 0x4000 ... 0x5fff:
      ramBank.raw = val & 3;
      break;
    case 0x6000 ... 0x7fff:
      mode = val & 1;
      break;
    }
  }

  void WriteERAM(half addr, byte val)
  {
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
          ram[0x2000 * ramBank.raw + (addr - 0xa000)] = val;
        }
        else
        {
          ram[addr - 0xa000] = val;
        }
      }
    }
  }

private:
  struct
  {
    unsigned raw : 5;
  } romBank;

  struct
  {
    unsigned raw : 2;
  } ramBank;
  bool mode = false;
  bool ramEnable = false;
  byte romSize = 0;
  byte ramSize = 0;
  const half bitmasks[7] = {0x1, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x1f};

  const word RAM_SIZES[6] = {0, 2 * 1024, 8 * 1024, 32 * 1024, 128 * 1024, 64 * 1024};

  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
};

class MBC3
{
public:
  MBC3() { std::fill(ram.begin(), ram.end(), 0); }

  void LoadROM(std::vector<byte>& rom) { this->rom = rom; }

  byte ReadROM(half addr)
  {
    switch (addr)
    {
    case 0 ... 0x3fff:
      return rom[addr];
    case 0x4000 ... 0x7fff:
      return rom[0x4000 * romBank + (addr - 0x4000)];
    }
  }

  byte ReadERAM(half addr)
  {
    if (!ramEnable)
    {
      return 0xff;
    }

    return ram[0x2000 * ramBank + (addr - 0xa000)];
  }

  void WriteROM(half addr, byte val)
  {
    switch (addr)
    {
    case 0 ... 0x1fff:
      ramEnable = ((val & 0xf) == 0x0a);
      break;
    case 0x2000 ... 0x3fff:
      romBank = val;
      break;
    case 0x4000 ... 0x5fff:
      if (val < 4)
      {
        ramBank = val;
      }
      break;
    }
  }

  void WriteERAM(half addr, byte val)
  {
    if (ramEnable)
    {
      ram[0x2000 * ramBank + (addr - 0xA000)] = val;
    }
  }

private:
  byte ramBank = 0;
  byte romBank = 0;
  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
  bool ramEnable = false;
};

class MBC5
{
public:
  MBC5()
  {
    std::fill(ram.begin(), ram.end(), 0);
    romBank.raw = 0;
    ramBank.raw = 0;
  }

  void LoadROM(std::vector<byte>& rom) { this->rom = rom; }

  byte ReadROM(half addr)
  {
    switch (addr)
    {
    case 0 ... 0x3fff:
      return rom[addr];
    case 0x4000 ... 0x7fff:
      return rom[0x4000 * romBank.raw + (addr - 0x4000)];
    }
  }

  byte ReadERAM(half addr)
  {
    if (ramEnable)
    {
      return ram[0x2000 * ramBank.raw + (addr - 0xa000)];
    }

    return 0xff;
  }

  void WriteROM(half addr, byte val)
  {
    switch (addr)
    {
    case 0 ... 0x1fff:
      ramEnable = ((val & 0xf) == 0x0a);
      break;
    case 0x2000 ... 0x2fff:
      romBank.low = val;
      break;
    case 0x3000 ... 0x3fff:
      romBank.high = val & 1;
      break;
    case 0x4000 ... 0x5fff:
      ramBank.raw = val & 0xf;
      break;
    }
  }

  void WriteERAM(half addr, byte val)
  {
    if (addr < 0xc000)
    {
      ram[0x2000 * ramBank.raw + (addr - 0xa000)] = val;
    }
  }

private:
  union
  {
    struct
    {
      unsigned low : 8;
      unsigned high : 1;
    };
    unsigned raw : 9;
  } romBank;

  struct
  {
    unsigned raw : 4;
  } ramBank;
  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
  bool ramEnable = false;
};

class Cart
{
public:
  Cart() {}

  void LoadROM(std::vector<byte>& rom)
  {
    romType = rom[0x147];
    printf("%s\n", mbcs[romType].c_str());
    mbc0.LoadROM(rom);
    mbc1.LoadROM(rom);
    mbc3.LoadROM(rom);
    mbc5.LoadROM(rom);
  }

  byte ReadROM(half addr)
  {
    switch (romType)
    {
    case 0:
      return mbc0.ReadROM(addr);
    case 1 ... 3:
      return mbc1.ReadROM(addr);
    case 0xf ... 0x13:
      return mbc3.ReadROM(addr);
    case 0x19 ... 0x1e:
      return mbc5.ReadROM(addr);
    }
  }

  byte ReadERAM(half addr)
  {
    switch (romType)
    {
    case 0:
      return mbc0.ReadERAM(addr);
    case 1 ... 3:
      return mbc1.ReadERAM(addr);
    case 0xf ... 0x13:
      return mbc3.ReadERAM(addr);
    case 0x19 ... 0x1e:
      return mbc5.ReadERAM(addr);
    }
  }

  void WriteROM(half addr, byte val)
  {
    switch (romType)
    {
    case 0:
      mbc0.WriteROM(addr, val);
      break;
    case 1 ... 3:
      mbc1.WriteROM(addr, val);
      break;
    case 0xf ... 0x13:
      mbc3.WriteROM(addr, val);
      break;
    case 0x19 ... 0x1e:
      mbc5.WriteROM(addr, val);
      break;
    }
  }

  void WriteERAM(half addr, byte val)
  {
    switch (romType)
    {
    case 0:
      mbc0.WriteERAM(addr, val);
      break;
    case 1 ... 3:
      mbc1.WriteERAM(addr, val);
      break;
    case 0xf ... 0x13:
      mbc3.WriteERAM(addr, val);
      break;
    case 0x19 ... 0x1e:
      mbc5.WriteERAM(addr, val);
      break;
    }
  }

private:
  byte romType = 0;
  NoMBC mbc0;
  MBC1 mbc1;
  MBC3 mbc3;
  MBC5 mbc5;
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
  Cart cart;

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