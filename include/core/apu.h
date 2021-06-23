#pragma once
#include "ch1.h"
#include "ch2.h"
#include "ch3.h"
#include "ch4.h"
#include "control.h"

namespace natsukashii::core
{
struct Apu {
	Apu(bool skip);
	~Apu();

	CH1 ch1;
	CH2 ch2;
	CH3 ch3;
	CH4 ch4;
	Control control;
	void Reset();
	void Step(u64 cycles);
	u8 ReadIO(u16 addr);
	void WriteIO(u16 addr, u8 val);
	void sample();
	bool skip;
	float sample_clock = 87.78;
	int buffer_pos = 0;
	std::array<u8, 1024> buffer{0};
	SDL_AudioDeviceID device;
	int frequency_timer;
};
} // natsukashii::core