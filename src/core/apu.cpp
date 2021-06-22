#include "apu.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace natsukashii::core
{
ma_device device;

Apu::~Apu() {
	ma_device_uninit(&device);
}



Apu::Apu(bool skip) : skip(skip)
{
	ma_device_config config;
	config = ma_device_config_init(ma_device_type_playback);
	config.playback.format = ma_format_u8;
	config.playback.channels = channels;
	config.dataCallback = 
	config.sampleRate = frequency;

	if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
		printf("Failed to open playback device\n");
		exit(1);
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		printf("Failed to start playback device.\n");
		ma_device_uninit(&device);
		exit(1);
	}

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

	sample_rate -= cycles;
	if(sample_rate <= 0) {
		sample();
		sample_rate += 4213440 / frequency;
	}
}

u16 Apu::reload_timer2() {
	return (2048 - ((nr24.freq << 8) | nr23)) * 2;
}

u8 Apu::sample_sq2() {
	u8 duty = duty_sq2[nr21.duty][ch2_duty_index];
	return nr22.volume * duty;
}

void Apu::sample() {
	u8 sample2 = sample_sq2();
	if (sample2 > 0.1) sample2 = 0.1;
	if (sample2 < -0.1) sample2 = -0.1;

	while(ma_device_start(&device) > (1024 * 4)) {
		
	}
  
	ma_device_stop(&device);
}
}