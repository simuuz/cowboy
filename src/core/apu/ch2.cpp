#include "ch2.h"

namespace natsukashii::core
{
CH2::CH2() {
  nr21.raw = 0;
  nr22.raw = 0;
  nr24.raw = 0;
}

void CH2::reset()
{
  nr21.raw = 0;
  nr22.raw = 0;
  nr24.raw = 0;
}

void CH2::step(u64 cycles) {
  timer -= cycles;
	if(timer <= 0) {
		duty_index = (duty_index + 1) % 8;
		timer += reload_timer();
	}
}

u8 CH2::sample() {
  u8 duty = this->duty[nr21.duty][duty_index];
  return nr22.volume * 0.25 * duty;
}

s16 CH2::reload_timer() {
  return (2048 - ((nr24.freq << 8) | nr23)) << 2;
}

u8 CH2::read(u16 addr) {
  switch(addr & 0xff) {
    case 0x16: return nr21.duty;
    case 0x17: return nr22.raw;
    case 0x19: return nr24.len_enable;
  }
}

void CH2::write(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x16: nr21.raw = val; break;
    case 0x17: nr22.raw = val; break;
    case 0x18: nr23 = val; break;
    case 0x19: nr24.raw = val; break;
  }
}
}