#pragma once
#include <array>
#include <iterator>
#include <vector>
#include "common.h"

constexpr int BOOTROM_SZ = 0x100;
constexpr int EXTRAM_SZ = 0x2000;
constexpr int ERAM_SZ = 0x1fff;
constexpr int WRAM_SZ = 0x2000;
constexpr int HRAM_SZ = 0x7f;
constexpr int ROM_SZ_MIN = 0x8000;

namespace natsukashii::core
{
class Cart
{
public:
  virtual byte Read(half addr) { return 0xff; }
  virtual void Write(half addr, byte val) { }
  virtual void Clear() { }
  virtual void Save(std::string filename) { }
};

class NoMBC : public Cart
{
public:
  NoMBC(std::vector<byte>& rom);
  byte Read(half addr);
  void Write(half addr, byte val);
  void Save(std::string filename) {}
private:
  std::vector<byte> rom;
};

class MBC1 : public Cart
{
public:
  MBC1(std::vector<byte>& rom, std::string savefile);
  byte Read(half addr);
  void Write(half addr, byte val);
  void Save(std::string filename);
private:
  byte romBank = 1;
  byte ramBank = 1;
  byte romSize;
  byte ramSize;
  bool mode = false;
  bool ramEnable = false;
  static constexpr half bitmasks[7] = {0x1, 0x3, 0x7, 0xf, 0x1f, 0x1f, 0x1f};
  static constexpr word RAM_SIZES[6] = {0, 2 * 1024, 8 * 1024, 32 * 1024, 128 * 1024, 64 * 1024};
  
  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
};

class MBC2 : public Cart
{
public:
  MBC2(std::vector<byte>& rom, std::string savefile);
  byte Read(half addr);
  void Write(half addr, byte val);
  void Save(std::string filename);
private:
  byte romBank = 1;
  bool ramEnable = false;

  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
};

class MBC3 : public Cart
{
public:
  MBC3(std::vector<byte>& rom, std::string savefile);
  byte Read(half addr);
  void Write(half addr, byte val);
  void Save(std::string filename);
private:
  byte ramBank = 0;
  byte romBank = 0;

  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
  bool ramEnable = false;
};

class MBC5 : public Cart
{
public:
  MBC5(std::vector<byte>& rom, std::string savefile);
  byte Read(half addr);
  void Write(half addr, byte val);
  void Save(std::string filename);
private:
  half romBank = 1;
  byte ramBank = 1;
  std::array<byte, ERAM_SZ> ram;
  std::vector<byte> rom;
  bool ramEnable = false;
};

class Mem
{
public:
  ~Mem();
  Mem(bool skip, std::string bootrom_path);
  void LoadROM(std::string filename);
  void Reset();
  byte Read(half addr);
  void Write(half addr, byte val);

  byte ie = 0;
  bool skip;

  struct Joypad
  {
    union
    {
      struct
      {
        unsigned:1;
        unsigned:1;
        unsigned select_btn:1;
        unsigned select_dpad:1;
        unsigned btn_start_down:1;
        unsigned btn_select_up:1;
        unsigned btn_a_left:1;
        unsigned btn_b_right:1;
      };

      byte raw;
    };

    Joypad() : raw(0xff) {}
    void write(byte val) {
      raw = val;
      raw |= (0b11000000);
    }
  };

  struct IO
  {
    byte bootrom = 1, tac = 0, tima = 0, tma = 0, intf = 0, div = 0;
    Joypad joy;
  } io;

  friend class Ppu;
  bool rom_opened = false;
  void DoInputs(int key, int action);
private:
  bool held = false;
  std::string savefile;
  Cart* cart = nullptr;
  void LoadBootROM(std::string filename);

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

  void HandleJoypad(byte val);
};
}  // namespace natsukashii::core