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

	u8 nr33 = 0;

	union {
		struct {
			unsigned freq:3;
			unsigned:3;
			unsigned len_enable:1;
			unsigned trigger:1;
		};

		u8 raw;
	} nr34;

	union {
		struct {
			unsigned lo:4;
			unsigned hi:4;
		};

		u8 raw;
	} wave_ram[16];

	u8 read(u16 addr) { return 0xff; }
	void write(u16 addr, u8 val) { }
};
}