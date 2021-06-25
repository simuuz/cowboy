#pragma once
#include "mem.h"

namespace natsukashii::core
{
struct CH3
{
  CH3();
	void reset();
	
 	union {
		struct {
			unsigned:7;
			unsigned enabled:1;
		};

		u8 raw;
	} nr30;

	u8 nr31 = 0;

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
			unsigned:3;
			unsigned:3;
			unsigned len_enable:1;
			unsigned enabled:1;
		};

		u8 raw;
	} nr34;

	u16 frequency = 0;
	s16 timer = 0;
	u8 wave_ram[16];
	u32 wave_pos = 0;
	bool dac = 0;
	u16 length_counter = 0;
	u8 output_level = 0;
	u8 vol_shift = 0;

	constexpr static u8 shifts[4] = {
		4, 0, 1, 2
	};

	void step_length();
	u8 sample();
	void tick();
	u8 read(u16 addr);
	void write(u16 addr, u8 val);
};
}