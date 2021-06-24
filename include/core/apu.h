#pragma once
#include "ch1.h"
#include "ch2.h"
#include "ch3.h"
#include "ch4.h"
#include "control.h"
constexpr int FREQUENCY = 96000;
constexpr int CHANNELS = 2;
constexpr int SAMPLES = 8192;

namespace natsukashii::core
{
struct Apu {
	~Apu();
	Apu(bool skip);
	void Reset();
	void Step(u8 cycles);

	CH1 ch1;
	CH2 ch2;
	CH3 ch3;
	CH4 ch4;
	Control control;
	u8 ReadIO(u16 addr);
	void WriteIO(u16 addr, u8 val);
	bool skip;
	u32 sample_clock = 0;
	int buffer_pos = 0;
	u8 frame_sequencer_position = 0;
	u16 buffer[SAMPLES * 2];
	SDL_AudioDeviceID device;
	int frequency_timer;
};
} // natsukashii::core