#pragma once
#include "ch1.h"
#include "ch2.h"
#include "ch3.h"
#include "ch4.h"
#include "control.h"
constexpr int FREQUENCY = 48000;
constexpr int CHANNELS = 2;
constexpr int SAMPLES = 4096;

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
	void Step(u8 cycles, bool unlocked);
	u8 ReadIO(u16 addr);
	void WriteIO(u16 addr, u8 val);
	bool skip;
	u32 sample_clock = 0;
	u32 counter = 4194300 / FREQUENCY;
	int buffer_pos = 0;
	u8 frame_sequencer_position = 0;
	float buffer[SAMPLES];
	SDL_AudioDeviceID device;
	int frequency_timer;
};
} // natsukashii::core