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

void CH2::step_length() {
  if(nr24.len_enable && nr21.len) {
    nr21.len--;
    if(nr21.len == 0) {
      nr24.enabled = 0;
    }
  }
}

void CH2::tick() {
  timer -= 4;
  
  if(timer <= 0) {
    duty_index = (duty_index + 1) & 7;
    timer = (2048 - (((u16)nr24.freq << 8) | nr23)) << 2;
  }
}

float CH2::sample() {
  if(nr24.enabled) {
    float duty = this->duty[nr21.duty][duty_index];
    return nr22.volume * 0.01 * duty;
  } else {
    return 0;
  }
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