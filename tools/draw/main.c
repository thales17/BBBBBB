#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 640
#define MINSIZE 2
#define MAXBLOCKS 320 * 320
#define BSIZE 2
#define COLS 320

#define TYPE_NOTHING 0
#define TYPE_SOMETHING 1

struct block {
	SDL_Rect r;
	int type;
};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

struct block blocks[MAXBLOCKS];

int bsize = 64;
int cols = 10;
int len = 100;

void setup()
{
	int i;

	for (i = 0; i < len; i++) {
		blocks[i].r.x = (i % cols) * bsize;
		blocks[i].r.y = (i / cols) * bsize;
		blocks[i].r.w = bsize;
		blocks[i].r.h = bsize;
	}
}



int handle_input()
{
	SDL_PumpEvents();
	int x, y;
	int count;
	int rx, ry;
	int idx;
	int last_idx;
	const Uint32 mstate = SDL_GetMouseState(&x, &y);
	const Uint8 *kstate = SDL_GetKeyboardState(NULL);
	if (kstate[SDL_SCANCODE_ESCAPE]) {
		return 1;
	}

	if (mstate & SDL_BUTTON_LEFT) {
		/*printf("x: %d, y: %d\n", x, y);*/
		rx = x / bsize;
		ry = y / bsize;
		/*printf("rx: %d, ry: %d\n", rx, ry);*/
		idx = ry * cols + rx;

		if (kstate[SDL_SCANCODE_S]) {
			blocks[idx].type = TYPE_SOMETHING;
		} else if (kstate[SDL_SCANCODE_N]) {
			blocks[idx].type = TYPE_NOTHING;
		}
	}

	if (kstate[SDL_SCANCODE_SPACE]) {
		printf("\n\n\n");
		count = 0;
		for (idx = 0; idx < len; idx++) {
			if (blocks[idx].type == TYPE_SOMETHING) {
				printf("%d,", idx);
				count++;
			}
		}
		printf(" Count: %d", count);
	}

	return 0;
}

void draw()
{
	int i;
	SDL_Rect r;

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (i = 0; i < len; i++) {
		switch (blocks[i].type) {
		case TYPE_SOMETHING:
			SDL_RenderFillRect(renderer, &blocks[i].r);
			break;
		}
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (i = 1; i < cols; i++) {
		r.x = i * bsize;
		r.y = 0;
		r.w = 1;
		r.h = HEIGHT;
		SDL_RenderFillRect(renderer, &r);
		r.x = 0;
		r.y = i * bsize;
		r.w = WIDTH;
		r.h = 1;
		SDL_RenderFillRect(renderer, &r);
	}
}

int main()
{
	int i;

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(
		"Draw",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_OPENGL
		);

	if (window == NULL) {
		SDL_Log("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		SDL_Log("Could not create renderer: %s\n", SDL_GetError());
		return 1;
	}

	setup();
	while (1) {
		if (handle_input()) {
			break;
		}


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		draw();
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
