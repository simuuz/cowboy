#include "mem.h"

namespace natsukashii::core
{
NoMBC::NoMBC(std::vector<byte>& rom) : rom(rom) {}

byte NoMBC::Read(half addr)
{
if(addr >= 0 && addr < 0x8000)
    return rom[addr];
return 0xff;
}

void NoMBC::Write(half addr, byte val) {}
} // natsukashii::core