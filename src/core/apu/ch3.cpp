#include "ch3.h"

namespace natsukashii::core
{
CH3::CH3()
{
  nr30.raw = 0;
  nr32.raw = 0;
  nr34.raw = 0;
	wave_pos = 0;
	dac = false;
	length_counter = 0;
	output_level = 0;
	vol_shift = 0;
	frequency = 0;
	timer = 0;

  memset(wave_ram, 0, 16);
}

void CH3::reset()
{
  nr30.raw = 0;
  nr32.raw = 0;
  nr34.raw = 0;
	wave_pos = 0;
	dac = false;
	length_counter = 0;
	output_level = 0;
	vol_shift = 0;
  frequency = 0;
  timer = 0;

  memset(wave_ram, 0, 16);
}

void CH3::tick() {
  if(timer == 0) {
    timer = (2048 - frequency) << 1;
    wave_pos = (wave_pos + 1) & 31;
  }

  timer--;
}

u8 CH3::sample() {
  if(dac && nr30.enabled) {
    u8 sample = (wave_ram[wave_pos >> 1] >> ((wave_pos & 1) != 0 ? 4 : 0)) & 0xF;
    return ((sample >> vol_shift) / 7) - 1;
  } else {
    return 0;
  }
}

void CH3::step_length() {
  if(nr34.len_enable && (length_counter > 0)) {
    length_counter--;
    if(length_counter == 0) {
      nr30.enabled = 0;
    }
  }
}

u8 CH3::read(u16 addr) {
  switch(addr & 0xff) {
    case 0x1a: return (dac << 7) | 0x7F;
    case 0x1b: return 0xff;
    case 0x1c: return (output_level << 5) | 0x9F;
    case 0x1d: return 0xff;
    case 0x1e: return ((u8)nr34.len_enable << 6) | 0xBF;
    case 0x30 ... 0x3F: return wave_ram[addr - 0xFF30];
  }
}

void CH3::write(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x1a:
      dac = ((val >> 7) & 1) != 0;
      if(!dac) {
        nr30.enabled = 0;
      }
      break;
    case 0x1b:
      length_counter = 256 - val;
      break;
    case 0x1c:
      output_level = (val >> 5) & 3;
      vol_shift = shifts[output_level];
      break;
    case 0x1d:
      frequency = (frequency & 0x700) | val;
      break;
    case 0x1e: {
      frequency = (frequency & 0xff) | ((val & 7) << 8);
      nr34.len_enable = (val >> 6) & 1;
      bool trigger = (val >> 7) != 0;
      if(trigger && dac) {
        nr30.enabled = 1;
        
        if(length_counter == 0) {
          length_counter = 256;
        }
      }
    } break;
    case 0x30 ... 0x3F: wave_ram[addr - 0xFF30] = val; break;
  }
}
}