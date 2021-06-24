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

void CH1::step_length() {
  if(nr14.len_enable && nr11.len) {
    nr11.len--;
    if(nr11.len == 0) {
      nr14.enabled = 0;
    }
  }
}

void CH1::step_sweep() {
  if(sweep_period_timer > 0) {
    sweep_period_timer--;
  }

  if(sweep_period_timer == 0) {
    sweep_period_timer = nr10.period > 0 ? nr10.period : 8;

    if(nr10.period > 0) {
      u16 new_freq = calculate_frequency();
      if(new_freq <= 2047 && nr10.sweep > 0) {
        nr14.freq = new_freq >> 5;
        nr13 = new_freq & 0xFF;
        shadow_frequency = new_freq;
        calculate_frequency();
      }
    }
  }
}

void CH1::tick() {
  timer -= 4;
  
  if(timer <= 0) {
    duty_index = (duty_index + 1) & 7;
    timer = (2048 - (((u16)nr14.freq << 8) | nr13)) << 2;
  }
}

float CH1::sample() {
  if(nr14.enabled) {
    float duty = this->duty[nr11.duty][duty_index];
    return nr12.volume * 0.01 * duty;
  } else {
    return 0;
  }
}

u16 CH1::calculate_frequency() {
  u16 new_frequency = shadow_frequency >> nr10.sweep;
  new_frequency = nr10.negate ? shadow_frequency - new_frequency : shadow_frequency + new_frequency;
  if(new_frequency > 2047) {
    nr14.enabled = 0;
  }

  return new_frequency;
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