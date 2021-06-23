#include "apu.h"

namespace natsukashii::core
{
constexpr int FREQUENCY = 48000;
constexpr int CHANNELS = 1;
constexpr int SAMPLES = 1024;

Apu::~Apu() {
	SDL_CloseAudioDevice(device);
}

Apu::Apu(bool skip) : skip(skip)
{
	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec spec = {
		.freq = FREQUENCY,
		.format = AUDIO_U8,
		.channels = CHANNELS,
		.silence = 0,
		.samples = SAMPLES,
		.padding = 0,
		.size = 0,
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
	SDL_CloseAudioDevice(device);
	SDL_AudioSpec spec = {
		.freq = FREQUENCY,
		.format = AUDIO_U8,
		.channels = CHANNELS,
		.silence = 0,
		.samples = SAMPLES,
		.padding = 0,
		.size = 0,
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

void Apu::Step(u64 cycles) {
	ch1.step(cycles);
	ch2.step(cycles);

	sample_clock -= cycles;
	if(sample_clock <= 0) {
		sample();
		sample_clock = 4194300 / FREQUENCY;
	}
}

void Apu::sample() {
	buffer[buffer_pos++] = ch1.sample() + ch2.sample();
	if(buffer_pos >= SAMPLES) {
		buffer_pos = 0;
		u32 len = SAMPLES * CHANNELS * sizeof(u8);
		while (SDL_GetQueuedAudioSize(device) > len * 4) {	}
		SDL_QueueAudio(device, buffer.data(), len);
	}
}

}