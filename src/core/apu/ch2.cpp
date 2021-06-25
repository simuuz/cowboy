#include "ch2.h"

namespace natsukashii::core
{
CH2::CH2()
{
  duty_index = 0;
	timer = 0;
  frequency = 0;
	period_timer = 0;
  current_volume = 0;
	length_counter = 0;
  nr21.raw = 0;
  nr22.raw = 0;
  nr24.raw = 0;
}

void CH2::reset()
{
	duty_index = 0;
	timer = 0;
  frequency = 0;
	period_timer = 0;
  length_counter = 0;
  current_volume = 0;
  nr21.raw = 0;
  nr22.raw = 0;
  nr24.raw = 0;
}

void CH2::step_length() {
  if(nr24.len_enable && length_counter) {
    length_counter--;
    if(length_counter == 0) {
      nr24.enabled = 0;
    }
  }
}

void CH2::step_volume() {
  if(nr22.period != 0) {
    if(period_timer > 0) {
      period_timer--;
    }

    if(period_timer == 0) {
      period_timer = nr22.period;
      if((current_volume < 0xF && nr22.add_mode) || (current_volume > 0 && !nr22.add_mode)) {
        if(nr22.add_mode) {
          current_volume++;
        } else {
          current_volume--;
        }
      }
    }
  }
}

void CH2::tick() {
  if(timer <= 0) {
    timer = (2048 - frequency) << 2;
    duty_index = (duty_index + 1) & 7;
  }

  timer--;
}

u8 CH2::sample() {
  if(nr24.enabled) {
    return current_volume * this->duty[nr21.duty][duty_index];
  } else {
    return 0;
  }
}

u8 CH2::read(u16 addr) {
  switch(addr & 0xff) {
    case 0x15: return 0xff;
    case 0x16:
      return (nr21.duty << 6) | 0x3F;
    case 0x17:
      return (nr22.volume << 4) | (nr22.add_mode ? 8 : 0) | nr22.period;
    case 0x18: return 0xff;
    case 0x19: return (nr24.len_enable << 6) | 0xBF;
  }
}

void CH2::write(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x15: break;
    case 0x16:
      nr21.duty = (val >> 6) & 3;
      length_counter = 64 - (val & 0x3F);
      break;
    case 0x17:
      nr22.add_mode = val >> 3;
      nr22.volume = val >> 4;
      nr22.period = val & 7;
      dac = (val >> 3) & 0x1F;
      if(dac == 0) {
        nr24.enabled = 0;
      }
      break;
    case 0x18:
      frequency = (frequency & 0x700) | val;
      break;
    case 0x19:
      frequency = (frequency & 0xFF) | ((val & 7) << 8);
      nr24.len_enable = val >> 6;
      bool trigger = val >> 7;

      if(trigger && dac != 0) {
        if(length_counter == 0) {
          length_counter = 64;
        }

        nr24.enabled = 1;
        period_timer = nr22.period;
        current_volume = nr22.volume;
      }
      break;
  }
}
}