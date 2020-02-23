#ifndef STARFIELD_H
#define STARFIELD_H

#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define STAR_COUNT 50
#define STAR_MAXSIZE 10

SDL_Rect stars[STAR_COUNT];
int width;
int height;

void setup_starfield(int w, int h)
{
	int i;

	width = w;
	height = h;
	srand(time(NULL));

	for (i = 0; i < STAR_COUNT; i++) {
		int s = rand() % STAR_MAXSIZE;
		stars[i].x = rand() % width;
		stars[i].y = rand() % height;
		stars[i].w = s;
		stars[i].h = s;
	}
}

void update_starfield()
{
	int i;

	for (i = 0; i < STAR_COUNT; i++) {
		int speed = 1;
		if (stars[i].w > (STAR_MAXSIZE / 3)) {
			speed++;
		}
		if (stars[i].w > 2 * (STAR_MAXSIZE / 3)) {
			speed++;
		}
		stars[i].x += speed;
		stars[i].x %= width;
	}
}

void draw_starfield(SDL_Renderer *renderer)
{
	SDL_Rect r;
	int i;

	r.x = 0;
	r.y = 0;
	r.w = width;
	r.h = height;

	SDL_SetRenderDrawColor(renderer, 0x0B, 0x0B, 0x0B, 0xFF);
	SDL_RenderFillRect(renderer, &r);

	SDL_SetRenderDrawColor(renderer, 0xCC, 0xCC, 0xCC, 0xFF);
	for (i = 0; i < STAR_COUNT; i++) {
		SDL_RenderFillRect(renderer, &stars[i]);
	}
}

#endif
