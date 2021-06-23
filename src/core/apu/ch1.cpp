#include "ch1.h"
#include <cmath>

namespace natsukashii::core
{
CH1::CH1()
{
  nr10.raw = 0;
  nr11.raw = 0;
  nr12.raw = 0;
  nr14.raw = 0;
}

void CH1::reset()
{
  nr10.raw = 0;
  nr11.raw = 0;
  nr12.raw = 0;
  nr14.raw = 0;
}

void CH1::step(u64 cycles) {
  timer -= cycles;
	if(timer <= 0) {
		duty_index = (duty_index + 1) % 8;
		timer += reload_timer();
	}
}

u8 CH1::sample()
{
  u8 duty = this->duty[nr11.duty][duty_index];
	return nr12.volume * 0.25 * duty;
}

s16 CH1::reload_timer() {
  s16 prev_freq = (2048 - ((nr14.freq << 8) | nr13)) << 1;
  s16 new_freq = nr10.negate ? prev_freq + (prev_freq / pow(2, nr10.sweep)) : prev_freq - (prev_freq / pow(2, nr10.sweep));
  return new_freq;
}

u8 CH1::read(u16 addr) {
  switch(addr & 0xff) {
    case 0x10: return nr10.raw;
    case 0x11: return nr11.duty;
    case 0x12: return nr12.raw;
    case 0x14: return nr14.len_enable;
  }
}

void CH1::write(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x10: nr10.raw = val; break;
    case 0x11: nr11.raw = val; break;
    case 0x12: nr12.raw = val; break;
    case 0x13: nr13 = val; break;
    case 0x14: nr14.raw = val; break;
  }
}
}