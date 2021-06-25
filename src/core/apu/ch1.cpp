#include "ch1.h"
#include <cmath>

namespace natsukashii::core
{
CH1::CH1()
{
	timer = 0;
  frequency = 0;
	sweep_period_timer = 0;
	period_timer = 0;
	dac = 0;
	length_counter = 0;
	sweep_enable = false;
  nr10.raw = 0;
  nr11.raw = 0;
  nr12.raw = 0;
  nr14.raw = 0;
}

void CH1::reset()
{
	timer = 0;
  frequency = 0;
	sweep_period_timer = 0;
	period_timer = 0;
	dac = 0;
	length_counter = 0;
	sweep_enable = false;
  nr10.raw = 0;
  nr11.raw = 0;
  nr12.raw = 0;
  nr14.raw = 0;
}

void CH1::step_length() {
  if(nr14.len_enable && length_counter > 0) {
    length_counter--;
    if(length_counter == 0) {
      nr14.enabled = 0;
    }
  }
}

void CH1::step_volume() {
  if(nr12.period != 0) {
    if(period_timer > 0) {
      period_timer--;
    }

    if(period_timer == 0) {
      period_timer = nr12.period;
      if((current_volume < 0xF && nr12.add_mode) || (current_volume > 0 && !nr12.add_mode)) {
        if(nr12.add_mode) {
          current_volume++;
        } else {
          current_volume--;
        }
      }
    }
  }
}

void CH1::tick() {  
  if(timer <= 0) {
    timer = (2048 - frequency) << 2;
    duty_index = (duty_index + 1) & 7;
  }
  
  timer--;
}

u8 CH1::sample() {
  if(dac && nr14.enabled) {
    return current_volume * this->duty[nr11.duty][duty_index];
  } else {
    return 0;
  }
}

void CH1::step_sweep() {
  if(sweep_period_timer > 0) {
    sweep_period_timer--;
  }

  if(sweep_period_timer == 0) {
    sweep_period_timer = nr10.period > 0 ? nr10.period : 8;

    if(sweep_enable && (nr10.period > 0)) {
      u16 new_freq = calculate_frequency();
      if((new_freq <= 2047) && (nr10.sweep > 0)) {
        frequency = new_freq;
        shadow_frequency = new_freq;
        calculate_frequency();
      }
    }
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
    case 0x10:
      return (nr10.period << 4) | (nr10.negate) ? 8 : 0 | nr10.sweep | 0x80;
    case 0x11:
      return (nr11.duty << 6) | 0x3F;
    case 0x12:
      return (nr12.volume << 4) | (nr12.add_mode ? 8 : 0) | nr12.period;
    case 0x13: return 0xff;
    case 0x14:
      return (nr14.len_enable << 6) | 0xBF;
  }
}

void CH1::write(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x10:
      nr10.negate = val >> 3;
      nr10.period = val >> 4;
      nr10.sweep = val & 7;
      break;
    case 0x11:
      nr11.duty = (val >> 6) & 3;
      length_counter = 64 - (val & 0x3F);
      break;
    case 0x12:
      nr12.add_mode = val >> 3;
      nr12.volume = val >> 4;
      nr12.period = val & 7;
      dac = (val >> 3) & 0x1F;
      if(dac == 0) {
        nr14.enabled = 0;
      }
      break;
    case 0x13:
      frequency = (frequency & 0x700) | val;
      break;
    case 0x14:
      frequency = (frequency & 0xff) | ((val & 7) << 8);
      nr14.len_enable = val >> 6;
      bool trigger = val >> 7;
      if(trigger && dac != 0) {
        if(length_counter == 0) {
          length_counter = 64;
        }
        nr14.enabled = 1;
        period_timer = nr12.period;
        current_volume = nr12.volume;
        shadow_frequency = frequency;
        sweep_period_timer = nr10.period > 0 ? nr10.period : 8;
        sweep_enable = (nr10.period > 0) || (nr10.sweep > 0);
        if(nr10.sweep > 0) {
          calculate_frequency();
        }
      }
      break;
  }
}
}