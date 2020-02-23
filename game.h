#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "level.h"
#include "starfield.h"
#include "fire.h"

#define WIDTH 640
#define HEIGHT 480
#define COLS 32
#define ROWS 24
#define SIZE WIDTH / 16
#define BSIZE WIDTH / COLS
#define MAXBLOCKS COLS *ROWS
#define SPEED SIZE / 8
#define GRAV_SPEED 8
#define UPDATEMS 16
#define MAX_PATTERN 50

#define TYPE_NOTHING 0
#define TYPE_PLATFORM 1
#define TYPE_START 2
#define TYPE_GOAL 3

#define BG_STARFIELD 1
#define BG_FIRE 2

struct block {
	SDL_Rect r;
	int type;
};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int gravity, resting;
SDL_Rect player;
SDL_Point start;
SDL_Rect goal;

struct block blocks[MAXBLOCKS];

int move_left, move_right;
Uint32 last_time, current_time;

int current_level = 0;

int player_sprite;
int move_count;

int edit_mode;

int should_flip;
int flipping;

int background;

Uint8 last_kstate[512];

void draw_grid_pattern(int *pattern, int len, int stride, int size, int x, int y, int r, int g, int b)
{
	int i;
	SDL_Rect rect;

	rect.w = size;
	rect.h = size;

	SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
	for (i = 0; i < len; i++) {
		rect.x = x + ((pattern[i] % stride) * size);
		rect.y = y + ((pattern[i] / stride) * size);
		SDL_RenderFillRect(renderer, &rect);
	}
}

void draw_platform(SDL_Rect *r)
{
	int len = 64;
	int pattern[64] = { 0, 1, 2, 3, 5, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 18, 20, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 33, 35, 36, 37, 38, 50, 51, 52, 53, 55, 56, 57, 58, 60, 61, 62, 63, 65, 66, 67, 68, 70, 71, 72, 73, 75, 76, 77, 78, 80, 81, 82, 83, 85, 86, 87, 88 };

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, r);
	draw_grid_pattern(pattern, len, 10, BSIZE / 10, r->x, r->y, 0x70, 0x70, 0x70);
}

void draw_goal(SDL_Rect *r)
{
	int len = 48;
	int pattern[48] = { 4, 5, 13, 14, 15, 16, 22, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 37, 42, 43, 44, 45, 46, 47, 52, 53, 54, 55, 56, 57, 62, 63, 64, 65, 66, 67, 72, 73, 74, 75, 76, 77, 83, 84, 85, 86, 94, 95  };

	draw_grid_pattern(pattern, len, 10, BSIZE / 10, r->x - 4, r->y - 4, 0x00, 0x00, 0x00);
	draw_grid_pattern(pattern, len, 10, BSIZE / 10, r->x, r->y, 0XED, 0xED, 0xF1);

}

void draw_start(SDL_Rect *r)
{
	int len = 48;
	int pattern[48] = { 4, 5, 13, 14, 15, 16, 22, 23, 24, 25, 26, 27, 32, 33, 34, 35, 36, 37, 42, 43, 44, 45, 46, 47, 52, 53, 54, 55, 56, 57, 62, 63, 64, 65, 66, 67, 72, 73, 74, 75, 76, 77, 83, 84, 85, 86, 94, 95  };

	draw_grid_pattern(pattern, len, 10, BSIZE / 10, r->x - 4, r->y - 4, 0x00, 0x00, 0x00);
	draw_grid_pattern(pattern, len, 10, BSIZE / 10, r->x, r->y, 0xFF, 0, 0xFF);

}

void draw_down_player(SDL_Rect *r)
{
	int len = 43;
	int pattern_1[43] = { 4, 5, 14, 15, 21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38, 41, 43, 44, 45, 46, 48, 51, 53, 54, 55, 56, 58, 63, 66, 73, 76, 83, 86, 87, 88, 91, 92, 93 };
	int pattern_2[43] = { 4, 5, 14, 15, 21, 22, 23, 24, 25, 26, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38, 41, 43, 44, 45, 46, 48, 51, 53, 54, 55, 56, 58, 63, 66, 73, 76, 81, 82, 83, 86, 96, 97, 98 };

	draw_grid_pattern((player_sprite) ? pattern_2 : pattern_1, len, 10, SIZE / 10, r->x - 4, r->y - 4, 0x00, 0x00, 0x00);
	draw_grid_pattern((player_sprite) ? pattern_2 : pattern_1, len, 10, SIZE / 10, r->x, r->y, 0x65, 0x63, 0xFF);
}

void draw_up_player(SDL_Rect *r)
{
	int len = 43;
	int pattern_1[43] = { 1, 2, 3, 13, 16, 17, 18, 23, 26, 33, 36, 41, 43, 44, 45, 46, 48, 51, 53, 54, 55, 56, 58, 61, 62, 63, 64, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78, 84, 85, 94, 95 };
	int pattern_2[43] = { 6, 7, 8, 11, 12, 13, 16, 23, 26, 33, 36, 41, 43, 44, 45, 46, 48, 51, 53, 54, 55, 56, 58, 61, 62, 63, 64, 65, 66, 67, 68, 71, 72, 73, 74, 75, 76, 77, 78, 84, 85, 94, 95 };

	draw_grid_pattern((player_sprite) ? pattern_2 : pattern_1, len, 10, SIZE / 10, r->x - 4, r->y - 4, 0x00, 0x00, 0x00);
	draw_grid_pattern((player_sprite) ? pattern_2 : pattern_1, len, 10, SIZE / 10, r->x, r->y, 0x65, 0x63, 0xFF);
}

void setup_background()
{
	switch (background) {
	case BG_STARFIELD:
		setup_starfield(WIDTH, HEIGHT);
		break;
	case BG_FIRE:
		setup_fire(WIDTH, HEIGHT);
		break;
	}
}

void update_background()
{
	switch (background) {
	case BG_STARFIELD:
		update_starfield();
		break;
	case BG_FIRE:
		update_fire();
		break;
	}
}

void draw_background()
{
	switch (background) {
	case BG_STARFIELD:
		draw_starfield(renderer);
		break;
	case BG_FIRE:
		draw_fire(renderer);
		break;
	}
}

#endif
