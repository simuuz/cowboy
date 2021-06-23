#include "apu.h"

namespace natsukashii::core
{
constexpr u16 duty_sq2[4][8] = {
	{0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 0}
};

constexpr int FREQUENCY = 48000;
constexpr int CHANNELS = 2;
constexpr int SAMPLES = 1024;

Apu::~Apu() {
	SDL_CloseAudioDevice(device);
}

Apu::Apu(bool skip) : skip(skip)
{
	SDL_Init(SDL_INIT_AUDIO);
	SDL_AudioSpec spec = {
		.freq = FREQUENCY,
		.format = AUDIO_U16SYS,
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

	nr13 = 0;
	nr22.raw = 0;
	nr23 = 0;
	nr33 = 0;
  nr42.raw = 0;
	nr43.raw = 0;
	nr10.raw = skip ? 0x80 : 0;
	nr11.raw = skip ? 0xBF : 0;
	nr12.raw = skip ? 0xF3 : 0;
	nr14.raw = skip ? 0xBF : 0;
	nr21.raw = skip ? 0x3F : 0;
	nr24.raw = skip ? 0xBF : 0;
	nr30.raw = skip ? 0x7F : 0;
	nr31 = skip ? 0xFF : 0;
	nr32.raw = skip ? 0x9F : 0;
	nr34.raw = skip ? 0xBF : 0;
	nr41.raw = skip ? 0xFF : 0;
	nr44.raw = skip ? 0xBF : 0;
	nr50.raw = skip ? 0x77 : 0;
	nr51.raw = skip ? 0xF3 : 0;
	nr52.raw = skip ? 0xF1 : 0;
}

void Apu::Reset()
{
	nr13 = 0;
	nr22.raw = 0;
	nr23 = 0;
	nr33 = 0;
  nr42.raw = 0;
	nr43.raw = 0;
	nr10.raw = skip ? 0x80 : 0;
	nr11.raw = skip ? 0xBF : 0;
	nr12.raw = skip ? 0xF3 : 0;
	nr14.raw = skip ? 0xBF : 0;
	nr21.raw = skip ? 0x3F : 0;
	nr24.raw = skip ? 0xBF : 0;
	nr30.raw = skip ? 0x7F : 0;
	nr31 = skip ? 0xFF : 0;
	nr32.raw = skip ? 0x9F : 0;
	nr34.raw = skip ? 0xBF : 0;
	nr41.raw = skip ? 0xFF : 0;
	nr44.raw = skip ? 0xBF : 0;
	nr50.raw = skip ? 0x77 : 0;
	nr51.raw = skip ? 0xF3 : 0;
	nr52.raw = skip ? 0xF1 : 0;
}

void Apu::WriteIO(u16 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x10: nr10.raw = val; break;
    case 0x11: nr11.raw = val; break;
    case 0x12: nr12.raw = val; break;
    case 0x13: nr13 = val; break;
    case 0x14: nr14.raw = val; break;
    case 0x16: nr21.raw = val; break;
    case 0x17: nr22.raw = val; break;
    case 0x18: nr23 = val; break;
    case 0x19: nr24.raw = val; break;
    case 0x1A ... 0x3F: break;
  }
}

u8 Apu::ReadIO(u16 addr) {
  switch(addr & 0xff) {
    case 0x10: return nr10.raw;
    case 0x11: return nr11.duty;
    case 0x12: return nr12.raw;
    case 0x14: return nr14.len_enable;
    case 0x16: return nr21.duty;
    case 0x17: return nr22.raw;
    case 0x19: return nr24.len_enable;
    case 0x1A ... 0x3F: return 0xff;
  }
}

void Apu::Step(u64 cycles) {
	timer2 -= cycles;
	if(timer2 <= 0) {
		ch2_duty_index = (ch2_duty_index + 1) % 8;
		timer2 += reload_timer2();
	}

	sample_clock -= cycles;
	if(sample_clock <= 0) {
		sample();
		sample_clock = 4194300 / FREQUENCY;
	}
}

u16 Apu::reload_timer2() {
	return (2048 - ((nr24.freq << 8) | nr23)) << 2;
}

u16 Apu::sample_sq2() {
	u16 duty = duty_sq2[nr21.duty][ch2_duty_index];
	return nr22.volume * duty;
}

void Apu::sample() {
	buffer[buffer_pos] = sample_sq2();
	buffer_pos++;
	if(buffer_pos >= SAMPLES) {
		buffer_pos = 0;
		SDL_QueueAudio(device, buffer.data(), SAMPLES * CHANNELS);
		while (SDL_GetQueuedAudioSize(device) > SAMPLES * CHANNELS * 4) {	}
	}
}

}