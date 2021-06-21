#pragma once
#include "mem.h"

namespace natsukashii::core
{
constexpr s8 duty_sq2[4][8] = {
	{-1,-1,-1,-1,-1,-1,-1,+1},
	{+1,-1,-1,-1,-1,-1,-1,+1},
	{+1,-1,-1,-1,-1,+1,+1,+1},
	{-1,+1,+1,+1,+1,+1,+1,-1}
};

class Apu {
public:
	Apu(bool skip);
	~Apu();
	void Reset();
	void Step(u64 cycles);
private:
	bool skip;
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

	constexpr static int frequency = 48000;
	constexpr static int samples = 512;
	constexpr static u8 channels = 2;
	constexpr static int maxSampleCycles = 4194304 / frequency;
	constexpr static int maxSequencerCycles = 4194304 / samples;

	u16 reload_timer2();
	s8 sample_sq2();
	void sample();
	u8 sample_rate = 88;
	SDL_AudioDeviceID audio_device;
	u8 ch2_duty_index;

	s16 timer2;
	int frequency_timer;
};
} // natsukashii::core