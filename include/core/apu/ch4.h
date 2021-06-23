#pragma once
#include "mem.h"

namespace natsukashii::core
{
struct CH4
{
  CH4();
	void reset();

	union {
		struct {
			unsigned len:6;
			unsigned:2;
		};

		u8 raw;
	} nr41;

	union {
		struct {
			unsigned sweep:3;
			unsigned dir:1;
			unsigned initial_vol:4;
		};

		u8 raw;
	} nr42;

	union {
		struct {
			unsigned ratio:3;
			unsigned step:1;
			unsigned freq:4;
		};

		u8 raw;
	} nr43;

	union {
		struct {
			unsigned:6;
			unsigned selection:1;
			unsigned trigger:1;
		};

		u8 raw;
	} nr44;
	
	u8 read(u16 addr) { return 0xff; }
	void write(u16 addr, u8 val) { }
};
}