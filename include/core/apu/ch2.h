#pragma once
#include "mem.h"

namespace natsukashii::core
{
struct CH2
{
  CH2();
  void reset();
  union {
		struct {
			unsigned len:6;
			unsigned duty:2;
		};
			
		u8 raw;
	} nr21;

  union {
		struct {
			unsigned period:3;
			unsigned add_mode:1;
			unsigned volume:4;
		};

		u8 raw;
	} nr22;

  u8 nr23 = 0;

  union {
		struct {
			unsigned freq:3;
			unsigned:3;
			unsigned len_enable:1;
			unsigned enabled:1;
		};

		u8 raw;
	} nr24;
  
	u8 duty_index = 0;

  constexpr static u8 duty[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0}
  };
	
	s16 timer;

  u8 sample();
	void step_length();
	void step_volume();
	void tick();

	u8 period_timer, current_volume;

  u8 read(u16 addr);
  void write(u16 addr, u8 val);
};
}