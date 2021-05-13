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

class NoMBC
{
public:
  NoMBC() {}

  void loadROM(std::vector<byte>& rom)
  {
    this->rom = rom;
  }

  byte readROM(half addr)
  {
    return rom[addr];
  }

  void writeROM(half addr, byte val)
  {
    printf("Wrote to ROM, addr: %08X\n", addr);
    exit(1);
  }

  byte readERAM(half addr)
  {
    return 0xff;
  }
  
  void writeERAM(half addr, byte val) {  }
private:
  std::vector<byte> rom;
};

class MBC1
{
public:
  MBC1()
  {
    std::fill(ram.begin(), ram.end(), 0);
  }

  void loadROM(std::vector<byte>& rom)
  {
    this->rom = rom;
    romSize = rom[0x148];
    ramSize = rom[0x149];
  }

  byte readROM(half addr)
  {
    byte zeroBank = 0;
    byte highBank = 0;
    switch(addr)
    {
    case 0 ... 0x3fff:
      if(mode)
      {
        switch (romSize)
        {
        case 0 ... 4:
          return rom[addr];
        case 5:
          setbit<byte, 5>(zeroBank, ramBank & 1);
          break;
        case 6:
          setbit<byte, 5>(zeroBank, ramBank >> 1);
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
      switch(romSize)
      {
        case 0 ... 4:
          highBank = romBank & bitmasks[romSize];
          break;
        case 5:
          highBank = romBank & bitmasks[romSize];
          setbit<byte, 5>(highBank, ramBank & 1);
          break;
        case 6:
          highBank = romBank & bitmasks[romSize];
          setbit<byte, 5>(highBank, ramBank & 1);
          setbit<byte, 6>(highBank, ramBank >> 1);
          break;
      }
      return rom[0x4000 * highBank + (addr - 0x4000)];
      break;
    }
  }
  
  byte readERAM(half addr)
  {
    if(ramEnable)
    {
      if(ramSize == 0x01 || ramSize == 0x02)
      {
        return ram[(addr - 0xa000) % (RAM_SIZES[ramSize])];
      }
      else if (ramSize == 0x03)
      {
        if(mode)
        {
          return ram[0x2000 * ramBank + (addr - 0xa000)];
        }
        else
        {
          return ram[addr - 0xa000];
        }
      }
    }

    return 0xff;
  }

  void writeROM(half addr, byte val)
  {
    switch(addr)
    {
    case 0 ... 0x1fff:
      ramEnable = ((val & 0xf) == 0xa);
      break;
    case 0x2000 ... 0x3fff:
      romBank = val & bitmasks[romSize];
      romBank = (val == 0) ? 1 : romBank;
      break;
    case 0x4000 ... 0x5fff:
      ramBank = val & 3;
      break;
    case 0x6000 ... 0x7fff:
      mode = val & 1;
      break;
    }
  }

  void writeERAM(half addr, byte val)
  {
    if(ramEnable)
    {
      if(ramSize == 0x01 || ramSize == 0x02)
      {
        ram[(addr - 0xa000) % (RAM_SIZES[ramSize])] = val;
      }
      else if (ramSize == 0x03)
      {
        if(mode)
        {
          ram[0x2000 * ramBank + (addr - 0xa000)] = val;
        }
        else
        {
          ram[addr - 0xa000] = val;
        }
      }
    }
  }
private:
  byte romBank = 0;
  byte ramBank = 0;
  bool mode = false;
  bool ramEnable = false;
  byte romSize = 0;
  byte ramSize = 0;
  const half bitmasks[7] = {
    0x1, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x1f
  };

  const word RAM_SIZES[6] = {
    0, 2 * 1024, 8 * 1024, 32 * 1024, 128 * 1024, 64 * 1024
  };

  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
};

class MBC3
{
public:
  MBC3()
  {
    std::fill(ram.begin(), ram.end(), 0);
  }

  void loadROM(std::vector<byte>& rom)
  {
    this->rom = rom;
  }

  byte readROM(half addr)
  {
    switch(addr)
    {
    case 0 ... 0x3fff:
      return rom[addr];
    case 0x4000 ... 0x7fff:
      return rom[0x4000 * romBank + (addr - 0x4000)];
    }
  }

  byte readERAM(half addr)
  {
    if(!ramEnable)
    {
      return 0xff;
    }

    return ram[0x2000 * ramBank + (addr - 0xa000)];
  }

  void writeROM(half addr, byte val)
  {
    switch(addr)
    {
    case 0 ... 0x1fff:
      ramEnable = ((val & 0xf) == 0x0a);
      break;
    case 0x2000 ... 0x3fff:
      romBank = val;
      break;
    case 0x4000 ... 0x5fff:
      if(val < 4)
      {
        ramBank = val;
      }
      break;
    }
  }

  void writeERAM(half addr, byte val)
  {
    if(ramEnable)
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
  }

  void loadROM(std::vector<byte>& rom)
  {
    this->rom = rom;
  }

  byte readROM(half addr)
  {
    switch (addr)
    {
    case 0 ... 0x3fff:
      return rom[addr];
    case 0x4000 ... 0x7fff:
      return rom[addr * (romBank.raw & 0x100) + (addr - 0x4000)];
    default:
      break;
    }
  }

  byte readERAM(half addr)
  {
    if(ramEnable)
    {
      return ram[0x2000 * ramBank.raw + (addr - 0xa000)];
    }

    return 0xff;
  }

  void writeROM(half addr, byte val)
  {
    switch(addr)
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

  void writeERAM(half addr, byte val)
  {
    if(addr < 0xc000)
    {
      ram[0x2000 * ramBank.raw + (addr - 0xa000)] = val;
    }
  }
private:
  union {
    struct {
      unsigned low:8;
      unsigned high:1;
    };
    unsigned raw:9;
  } romBank;

  struct {
    unsigned raw:4;
  } ramBank;
  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
  bool ramEnable = false;
};

class Cart
{
public:
  Cart() { }

  void loadROM(std::vector<byte>& rom)
  {
    romType = rom[0x147];
    printf("%s\n", mbcs[romType].c_str());
    mbc0.loadROM(rom);
    mbc1.loadROM(rom);
    mbc3.loadROM(rom);
    mbc5.loadROM(rom);
  }

  byte readROM(half addr)
  {
    switch(romType)
    {
    case 0:
      return mbc0.readROM(addr);
    case 1 ... 3:
      return mbc1.readROM(addr);
    case 0xf ... 0x13:
      return mbc3.readROM(addr);
    case 0x19 ... 0x1e:
      return mbc5.readROM(addr);
    }
  }

  byte readERAM(half addr)
  {
    switch(romType)
    {
    case 0:
      return mbc0.readERAM(addr);
    case 1 ... 3:
      return mbc1.readERAM(addr);
    case 0xf ... 0x13:
      return mbc3.readERAM(addr);
    case 0x19 ... 0x1e:
      return mbc5.readERAM(addr);
    }
  }

  void writeROM(half addr, byte val)
  {
    switch(romType)
    {
    case 0:
      mbc0.writeROM(addr, val);
      break;
    case 1 ... 3:
      mbc1.writeROM(addr, val);
      break;
    case 0xf ... 0x13:
      mbc3.writeROM(addr, val);
      break;
    case 0x19 ... 0x1e:
      mbc5.writeROM(addr, val);
      break;
    }
  }
  
  void writeERAM(half addr, byte val) {
    switch(romType)
    {
    case 0:
      mbc0.writeERAM(addr, val);
      break;
    case 1 ... 3:
      mbc1.writeERAM(addr, val);
      break;
    case 0xf ... 0x13:
      mbc3.writeERAM(addr, val);
      break;
    case 0x19 ... 0x1e:
      mbc5.writeERAM(addr, val);
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
  void load_rom(std::string filename);
  bool load_bootrom(std::string filename);
  void reset();
  byte read(half addr);
  void write(half addr, byte val);

  byte ie = 0;
  bool skip;

  struct IO
  {
    byte bootrom = 1, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
    void handle_joypad(byte val);
  } io;
  friend class Ppu;
  bool rom_opened = false;

private:
  Cart cart;
  Cart createCart(std::vector<byte>& rom);
  void write_io(half addr, byte val);
  byte read_io(half addr);
  byte getIF();
  byte bootrom[BOOTROM_SZ];
  byte extram[EXTRAM_SZ];
  byte wram[WRAM_SZ];
  byte eram[ERAM_SZ];
  byte hram[HRAM_SZ];
  byte MBC = 0;
  std::vector<byte> rom;
};