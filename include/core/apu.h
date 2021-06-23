#pragma once
#include "mem.h"

namespace natsukashii::core
{
struct Apu {
	Apu(bool skip);
	~Apu();

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
	} nr11, nr21;

	union {
		struct {
			unsigned period:3;
			unsigned add_mode:1;
			unsigned volume:4;
		};

		u8 raw;
	} nr12, nr22, nr42;
	
	u8 nr13, nr23, nr33;

	union {
		struct {
			unsigned freq:3;
			unsigned:3;
			unsigned len_enable:1;
			unsigned trigger:1;
		};

		u8 raw;
	} nr14, nr24, nr34;

	union {
		struct {
			unsigned:7;
			unsigned e:1;
		};

		u8 raw;
	} nr30;

	u8 nr31;

	union {
		struct {
			unsigned:5;
			unsigned vol_code:2;
			unsigned:1;
		};

		u8 raw;
	} nr32;

	union {
		struct {
			unsigned len:6;
			unsigned:2;
		};

		u8 raw;
	} nr41;

	union {
		struct {
			unsigned divisor:3;
			unsigned width_mode:1;
			unsigned shift:4;
		};

		u8 raw;
	} nr43;


	union {
		struct {
			unsigned:6;
			unsigned len_enable:1;
			unsigned trigger:1;
		};

		u8 raw;
	} nr44;

	union {
		struct {
			unsigned r_vol:3;
			unsigned r_enable:1;
			unsigned l_vol:3;
			unsigned l_enable:1;
		};

		u8 raw;
	} nr50;

	union {
		struct {
			unsigned sq1_r_enable:1;
			unsigned sq2_r_enable:1;
			unsigned wave_r_enable:1;
			unsigned noise_r_enable:1;
			unsigned sq1_l_enable:1;
			unsigned sq2_l_enable:1;
			unsigned wave_l_enable:1;
			unsigned noise_l_enable:1;
		};

		u8 raw;
	} nr51;

	union {
		struct {
			unsigned sq1_on:1;
			unsigned sq2_on:1;
			unsigned wave_on:1;
			unsigned noise_on:1;
			unsigned:3;
			unsigned power:1;
		};

		u8 raw;
	} nr52;

	void Reset();
	void Step(u64 cycles);
	u8 ReadIO(u16 addr);
	void WriteIO(u16 addr, u8 val);
	u16 reload_timer2();
	s16 sample_sq2();
	void sample();
	bool skip;
	float sample_clock = 87.78;
	u8 ch2_duty_index;
	int buffer_pos = 0;
	s16 buffer[1024]{0};
	SDL_AudioDeviceID device;
	s16 timer2;
	int frequency_timer;
};
} // natsukashii::core