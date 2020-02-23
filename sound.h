#include <SDL2/SDL.h>

struct squarewave {
	float phase_inc;
	float phase;
	float volume;
};

SDL_AudioDeviceID audio_dev;
SDL_AudioSpec have_spec;

struct squarewave synthdata[4];

struct sndinfo {
	uint8_t tone;
	uint32_t duration_ms;
};

int mute_after(void *data)
{
	struct sndinfo *mute_data = (struct sndinfo *)data;

	if (mute_data->tone >= 4) {
		SDL_Log("Invalid tone");
		return 1;
	}

	SDL_Delay(mute_data->duration_ms);

	SDL_LockAudioDevice(audio_dev);
	synthdata[mute_data->tone].volume = 0;
	SDL_UnlockAudioDevice(audio_dev);

	free(mute_data);
	return 0;
}

int play_sound(uint8_t tone, float volume, uint32_t duration_ms)
{
	struct sndinfo *mute_data = malloc(sizeof(struct sndinfo));

	mute_data->tone = tone;
	mute_data->duration_ms = duration_ms;

	if (tone >= 4) {
		SDL_Log("Invalid tone");
		return 1;
	}

	SDL_LockAudioDevice(audio_dev);
	synthdata[tone].volume = volume;
	SDL_UnlockAudioDevice(audio_dev);

	SDL_CreateThread(mute_after, NULL, (void *)mute_data);

	return 0;
}


void audio_callback(void *userdata, uint8_t *stream, int len)
{
	int i;
	int j;
	union floatbytes {
		float f;
		uint8_t bytes[4];
	};

	union floatbytes overlay;
	struct squarewave *swdata = (struct squarewave *)userdata;

	for (i = 0; i < len; i += 4) {
		overlay.f = 0;
		for (j = 0; j < 4; j++) {
			overlay.f += (swdata[j].phase < 0.5) ? swdata[j].volume : -1 * swdata[j].volume;

		}

		for (j = 0; j < 4; j++) {
			stream[i + j] = overlay.bytes[j];
		}

		for (j = 0; j < 4; j++) {
			swdata[j].phase = fmodf((swdata[j].phase + swdata[j].phase_inc), (float)1.0);
		}
	}
}

int init_audio()
{
	SDL_AudioSpec want;

	synthdata[0].phase_inc = 55.0 / 44100.0;
	synthdata[0].phase = 0.0;
	synthdata[0].volume = 0.0;

	synthdata[1].phase_inc = 110.0 / 44100.0;
	synthdata[1].phase = 0.0;
	synthdata[1].volume = 0.0;

	synthdata[2].phase_inc = 220.0 / 44100.0;
	synthdata[2].phase = 0.0;
	synthdata[2].volume = 0.0;

	synthdata[3].phase_inc = 440.0 / 44100.0;
	synthdata[3].phase = 0.0;
	synthdata[3].volume = 0.0;


	SDL_memset(&want, 0, sizeof(want));
	want.freq = 44100;
	want.format = AUDIO_F32;
	want.channels = 1;
	want.samples = 2048;
	want.userdata = &synthdata;
	want.callback = audio_callback;

	audio_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

	SDL_Log("freq: %d, format: %d, channels: %d, samples: %d, size: %d", have_spec.freq, have_spec.format, have_spec.channels, have_spec.samples, have_spec.size);
	if (audio_dev == 0) {
		SDL_Log("Unable to open audio device: %s", SDL_GetError());
		return 1;
	}

	SDL_PauseAudioDevice(audio_dev, 0);

	return 0;
}

void clean_up_sound()
{
	SDL_PauseAudioDevice(audio_dev, 1);
}
