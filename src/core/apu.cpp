#include "apu.h"

namespace natsukashii::core
{
Apu::~Apu() {
	SDL_CloseAudioDevice(device);
}

Apu::Apu(bool skip) : skip(skip)
{
	memset(buffer, 0, SAMPLES);
	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec spec = {
		.freq = FREQUENCY,
		.format = AUDIO_F32SYS,
		.channels = CHANNELS,
		.samples = SAMPLES,
		.callback = NULL,
		.userdata = NULL,
	};

	device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
	if (device == 0) {
		printf("Failed to open audio device: %s\n", SDL_GetError());
		exit(1);
	}
	
	SDL_PauseAudioDevice(device, 0);
}

void Apu::Reset()
{
	memset(buffer, 0, SAMPLES);
	SDL_CloseAudioDevice(device);
	SDL_AudioSpec spec = {
		.freq = FREQUENCY,
		.format = AUDIO_F32SYS,
		.channels = CHANNELS,
		.samples = SAMPLES,
		.callback = NULL,
		.userdata = NULL,
	};

	device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
	if (device == 0) {
		printf("Failed to open audio device: %s\n", SDL_GetError());
		exit(1);
	}
  
	SDL_PauseAudioDevice(device, 0);
}

void Apu::WriteIO(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x10 ... 0x14: ch1.write(addr, val); break;
    case 0x16 ... 0x19: ch2.write(addr, val); break;
    case 0x1A ... 0x1E: case 0x30 ... 0x3f:
			ch3.write(addr, val);
			break;
		case 0x20 ... 0x23: ch4.write(addr, val); break;
		case 0x24 ... 0x26: control.write(addr, val); break;
  }
}

u8 Apu::ReadIO(u16 addr) {
  switch(addr & 0xff) {
    case 0x10 ... 0x14: return ch1.read(addr);
    case 0x16 ... 0x19: return ch2.read(addr);
    case 0x1A ... 0x1E: case 0x30 ... 0x3f:
			return ch3.read(addr);
		case 0x20 ... 0x23: return ch4.read(addr);
		case 0x24 ... 0x26: return control.read(addr);
  }
}

void Apu::Step(u8 cycles, bool unlocked) {
	for(int i = 0; i < cycles; i++) {
		sample_clock++;
		ch1.tick();
		ch2.tick();

		if(sample_clock >= 8192) {
			sample_clock = 0;
			switch(frame_sequencer_position) {
				case 0:
				ch1.step_length();
				ch2.step_length();
				break;
				case 1: case 5: break;
				case 2:
				ch1.step_length();
				ch2.step_length();
				ch1.step_sweep();
				break;
				case 4:
				ch1.step_length();
				ch2.step_length();
				break;
				case 6:
				ch1.step_length();
				ch2.step_length();
				ch1.step_sweep();
				break;
			}

			frame_sequencer_position = (frame_sequencer_position + 1) & 7;
		}
		
		if(sample_clock % (4194300 / FREQUENCY) == 0) {
			buffer[buffer_pos++] = control.nr50.l_vol * (ch1.sample() + ch2.sample());
			buffer[buffer_pos++] = control.nr50.r_vol * (ch1.sample() + ch2.sample());
		}

		if(!unlocked && buffer_pos >= SAMPLES) {
			buffer_pos = 0;
			u32 len = SAMPLES * CHANNELS * sizeof(float);
			while(SDL_GetQueuedAudioSize(device) > len * 4) {	}
			SDL_QueueAudio(device, buffer, len);
		}
	}
}
}