#ifndef FIRE_H
#define FIRE_H

#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define FIRE_SIZE 2
#define FIRE_WIDTH 320
#define FIRE_HEIGHT 240

int width;
int height;

static unsigned char palette[37][3] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00 },
	{ 0x01, 0x02, 0x04 },
	{ 0x06, 0x08, 0x10 },
	{ 0x0A, 0x0D, 0x1D },
	{ 0x0D, 0x12, 0x2A },
	{ 0x11, 0x19, 0x35 },
	{ 0x15, 0x1E, 0x42 },
	{ 0x19, 0x24, 0x4E },
	{ 0x1D, 0x2A, 0x5B },
	{ 0x20, 0x30, 0x66 },
	{ 0x25, 0x35, 0x73 },
	{ 0x28, 0x3B, 0x7F },
	{ 0x2C, 0x40, 0x8B },
	{ 0x30, 0x46, 0x97 },
	{ 0x33, 0x4C, 0xA4 },
	{ 0x3D, 0x57, 0xA9 },
	{ 0x49, 0x63, 0xAE },
	{ 0x53, 0x70, 0xB3 },
	{ 0x5E, 0x7D, 0xB7 },
	{ 0x69, 0x89, 0xBD },
	{ 0x75, 0x96, 0xC1 },
	{ 0x80, 0xA3, 0xC6 },
	{ 0x8A, 0xB1, 0xCB },
	{ 0x95, 0xBD, 0xD0 },
	{ 0xA1, 0xC3, 0xD5 },
	{ 0xAE, 0xCB, 0xDC },
	{ 0xB9, 0xD3, 0xE1 },
	{ 0xC5, 0xDA, 0xE5 },
	{ 0xD0, 0xE2, 0xEB },
	{ 0xDD, 0xE9, 0xF0 },
	{ 0xE9, 0xF1, 0xF5 },
	{ 0xF5, 0xF9, 0xFB },
	{ 0xFF, 0xFF, 0xFF },
	{ 0xFF, 0xFF, 0xFF },
	{ 0xFF, 0xFF, 0xFF }
};

static unsigned char fire_pixels[FIRE_WIDTH * FIRE_HEIGHT];

void setup_fire(int w, int h)
{
	int i;
	int j;

	width = w;
	height = h;
	srand(time(NULL));



	for (i = 0; i < FIRE_WIDTH; i++) {
		for (j = 0; j < FIRE_HEIGHT - 1; j++) {
			fire_pixels[i] = 0;
		}
	}
	for (i = 0; i < FIRE_WIDTH; i++) {
		fire_pixels[(FIRE_HEIGHT - 1) * FIRE_WIDTH + i] = 36;
	}
}

void spread_fire(int src)
{
	int pixel = fire_pixels[src];

	if (pixel == 0) {
		fire_pixels[src - FIRE_WIDTH] = 0;
	} else {
		int randIdx = (rand() % 3) & 3;
		int dst = src - randIdx + 1;
		fire_pixels[dst - FIRE_WIDTH] = pixel - (randIdx & 1);
	}
}

void update_fire()
{
	int x;
	int y;

	for (x = 0; x < FIRE_WIDTH; x++) {
		for (y = 1; y < FIRE_HEIGHT; y++) {
			spread_fire(y * FIRE_WIDTH + x);
		}
	}
}

void draw_fire(SDL_Renderer *renderer)
{
	int i, j, count, r, g, b;
	SDL_Rect rects[FIRE_WIDTH * FIRE_HEIGHT];

	for (i = 1; i < 37; i++) {
		count = 0;
		for (j = 0; j < FIRE_WIDTH * FIRE_HEIGHT; j++) {
			if (fire_pixels[j] == i) {
				rects[count].x = j % FIRE_WIDTH * FIRE_SIZE;
				rects[count].y = j / FIRE_WIDTH * FIRE_SIZE;
				rects[count].w = FIRE_SIZE;
				rects[count].h = FIRE_SIZE;
				count++;
			}
		}
		r = palette[i][0];
		g = palette[i][1];
		b = palette[i][2];
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		SDL_RenderFillRects(renderer, rects, count);
	}
}
#endif
