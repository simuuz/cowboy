#pragma once
#include "mem.h"

namespace natsukashii::core
{
struct CH1
{
  CH1();
	void reset();

	union {
		struct {
			unsigned sweep:3;
			unsigned negate:1;
			unsigned period:3;
			unsigned:1;
		};

		u8 raw;
	} nr10;

  union {
		struct {
			unsigned len:6;
			unsigned duty:2;
		};
			
		u8 raw;
	} nr11;

  union {
		struct {
			unsigned period:3;
			unsigned direction:1;
			unsigned volume:4;
		};

		u8 raw;
	} nr12;

  u8 nr13 = 0;

  union {
		struct {
			unsigned freq:3;
			unsigned:3;
			unsigned len_enable:1;
			unsigned enabled:1;
		};

		u8 raw;
	} nr14;

	u8 duty_index = 0;

  constexpr static float duty[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0.1},
    {0.1, 0, 0, 0, 0, 0, 0, 0.1},
    {0.1, 0, 0, 0, 0, 0.1, 0.1, 0.1},
    {0, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0}
  };

	s16 timer;

	u8 sweep_period_timer;

	u8 read(u16 addr);
	void write(u16 addr, u8 val);
	void step_length();
	void step_sweep();
	void tick();

	u16 shadow_frequency;

	float sample();
	u16 calculate_frequency();
};
}