#pragma once
#include "mem.h"

namespace natsukashii::core
{
struct Control
{
  Control();
  void reset();

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

  u8 read(u16 addr) { return 0xff; }
  void write(u16 addr, u8 val) { }
};
}