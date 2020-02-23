#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "sound.h"
#include "game.h"

int charnum(char v)
{
	return v >= 48 && v <= 57;
}

int nextint(char *str, int start, int *end_idx)
{
	int len;
	int i;
	int j;
	int nint;
	char scopy[10];

	memset(scopy, '\0', 10);

	len = strlen(str);

	if (start >= len) {
		return -1;
	}

	for (i = start; i < len; i++) {
		j = i;
		while (charnum(str[j]) && j < len) {
			j++;
		}

		if (j > i) {
			strncpy(scopy, str + i, (j - i));
			nint = atoi(scopy);
			*end_idx = j;
			return nint;
		}
	}

	return -1;
}

void setup()
{
	int i;
	int j;
	int len;
	char *level;

	player.x = (WIDTH - SIZE) / 2;
	player.y = (HEIGHT - SIZE) / 2;
	player.w = SIZE;
	player.h = SIZE;
	i = 0;
	j = 0;

	for (i = 0; i < MAXBLOCKS; i++) {
		blocks[i].type = TYPE_NOTHING;
		blocks[i].r.x = (i % COLS) * BSIZE;
		blocks[i].r.y = (i / COLS) * BSIZE;
		blocks[i].r.w = BSIZE;
		blocks[i].r.h = BSIZE;
	}

	level = levels[current_level];
	len = strlen(level);
	i = 0;

	if (level[0] != 'S') {
		printf("No starting point found\n");
		while (i < len && level[i] != 'P') {
			i++;
		}

		if (i == len) {
			printf("No platforms found\n");
		}
	} else {
		j = nextint(level, i, &i);
		if (j != -1 && j >= 0 && j < MAXBLOCKS) {
			blocks[j].type = TYPE_START;
			start.x = (j % COLS) * BSIZE;
			start.y = (j / COLS) * BSIZE;
		}
	}

	while (i < len && level[i] != 'G') {
		j = nextint(level, i, &i);
		if (j != -1 && j >= 0 && j < MAXBLOCKS) {
			blocks[j].type = TYPE_PLATFORM;
		} else {
			break;
		}
		i++;
	}

	if (level[i] == 'G') {
		j = nextint(level, i, &i);
		if (j != -1 && j >= 0 && j < MAXBLOCKS) {
			blocks[j].type = TYPE_GOAL;
			goal.x = (j % COLS) * BSIZE;
			goal.y = (j / COLS) * BSIZE;
			goal.w = BSIZE;
			goal.h = BSIZE;
		}
	}

	player.x = start.x;
	player.y = start.y;
	move_left = 0;
	move_right = 0;
	last_time = 0;
	current_time = 0;
	gravity = 1;
	resting = 0;
	should_flip = 0;

	background = BG_STARFIELD;
	if (current_level == LEVEL_COUNT - 1) {
		background = BG_FIRE;
	}
	setup_background();
}

void setup_edit()
{
	int i;

	player.x = 0;
	player.y = 0;
	player.w = SIZE;
	player.h = SIZE;
	gravity = 1;

	for (i = 0; i < MAXBLOCKS; i++) {
		blocks[i].r.x = (i % COLS) * BSIZE;
		blocks[i].r.y = (i / COLS) * BSIZE;
		blocks[i].r.w = BSIZE;
		blocks[i].r.h = BSIZE;

	}
}

int handle_input()
{
	SDL_PumpEvents();

	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE]) {
		return 1;
	}

	if (state[SDL_SCANCODE_LCTRL] && state[SDL_SCANCODE_E] &&
	    !(last_kstate[SDL_SCANCODE_LCTRL] && last_kstate[SDL_SCANCODE_E])) {
		edit_mode = 1;
		SDL_ShowCursor(SDL_ENABLE);
		memcpy(last_kstate, state, 512);
		return 0;
	}

	if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
		move_left = 1;

	} else {
		move_left = 0;
	}

	if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
		move_right = 1;
	} else {
		move_right = 0;
	}

	if (state[SDL_SCANCODE_SPACE] && !last_kstate[SDL_SCANCODE_SPACE]) {
		if (!flipping) {
			should_flip = 1;
			flipping = 1;
		}
	}
	memcpy(last_kstate, state, 512);

	return 0;
}

int handle_input_edit()
{
	SDL_PumpEvents();
	int x, y;
	int rx, ry;
	int idx;
	int last_idx;
	const Uint32 mstate = SDL_GetMouseState(&x, &y);
	const Uint8 *kstate = SDL_GetKeyboardState(NULL);
	if (kstate[SDL_SCANCODE_ESCAPE]) {
		return 1;
	}

	if (kstate[SDL_SCANCODE_LCTRL] && kstate[SDL_SCANCODE_E] &&
	    !(last_kstate[SDL_SCANCODE_LCTRL] && last_kstate[SDL_SCANCODE_E])) {
		edit_mode = 0;
		SDL_ShowCursor(SDL_DISABLE);
		memcpy(last_kstate, kstate, 512);
		return 0;
	}

	if (mstate & SDL_BUTTON_LEFT) {
		/*printf("x: %d, y: %d\n", x, y);*/
		rx = x / 20;
		ry = y / 20;
		/*printf("rx: %d, ry: %d\n", rx, ry);*/
		idx = ry * COLS + rx;

		if (kstate[SDL_SCANCODE_B]) {
			blocks[idx].type = TYPE_PLATFORM;
		} else if (kstate[SDL_SCANCODE_G]) {
			last_idx = (goal.y / 20) * COLS + (goal.x / 20);
			blocks[last_idx].type = TYPE_NOTHING;
			blocks[idx].type = TYPE_GOAL;
			goal.x = rx * 20;
			goal.y = ry * 20;
		} else if (kstate[SDL_SCANCODE_P]) {
			last_idx = (player.x / 20) * COLS + (player.y / 20);
			blocks[idx].type = TYPE_NOTHING;
			blocks[idx].type = TYPE_START;
			player.x = rx * 20;
			player.y = ry * 20;
		} else if (kstate[SDL_SCANCODE_N]) {
			blocks[idx].type = TYPE_NOTHING;
		}
	}

	if (kstate[SDL_SCANCODE_SPACE] && !last_kstate[SDL_SCANCODE_SPACE]) {
		printf("\n\n\n");
		printf("S");
		for (idx = 0; idx < MAXBLOCKS; idx++) {
			if (blocks[idx].type == TYPE_START) {
				printf("%d,", idx);
				break;
			}
		}
		printf("P");
		for (idx = 0; idx < MAXBLOCKS; idx++) {
			if (blocks[idx].type == TYPE_PLATFORM) {
				printf("%d,", idx);
			}
		}
		printf("G");
		for (idx = 0; idx < MAXBLOCKS; idx++) {
			if (blocks[idx].type == TYPE_GOAL) {
				printf("%d", idx);
				break;
			}
		}
	}
	memcpy(last_kstate, kstate, 512);

	return 0;
}

void update()
{
	int i;

	if (move_left) {
		player.x -= SPEED;
		resting = 0;
		for (i = 0; i < MAXBLOCKS; i++) {
			if (blocks[i].type != TYPE_PLATFORM) {
				continue;
			}

			if (SDL_HasIntersection(&player, &blocks[i].r)) {
				player.x = blocks[i].r.x + blocks[i].r.w;
				break;
			}

		}
		move_count++;

	}

	if (move_right) {
		player.x += SPEED;
		resting = 0;
		for (i = 0; i < MAXBLOCKS; i++) {
			if (blocks[i].type != TYPE_PLATFORM) {
				continue;
			}

			if (SDL_HasIntersection(&player, &blocks[i].r)) {
				player.x = blocks[i].r.x - SIZE;
				break;
			}

		}
		move_count++;
	}

	if (move_count > 0 && move_count % 10 == 0) {
		move_count = 0;
		player_sprite++;
		player_sprite %= 2;
	}

	/* Fall logic */
	if (!resting) {
		player.y += (GRAV_SPEED * gravity);
		flipping = 1;
		if (player.x < 0) {
			player.x = 0;
		}

		if (player.x > WIDTH - SIZE) {
			player.x = WIDTH - SIZE;
		}

		for (i = 0; i < MAXBLOCKS; i++) {
			if (blocks[i].type != TYPE_PLATFORM) {
				continue;
			}

			if (SDL_HasIntersection(&player, &blocks[i].r)) {
				resting = 1;
				flipping = 0;
				if (gravity > 0) {
					player.y = blocks[i].r.y - SIZE;
				} else {
					player.y = blocks[i].r.y + blocks[i].r.h;
				}
			}

		}

		if (player.y < 0) {
			player.x = start.x;
			player.y = start.y;
			gravity = 1;
			resting = 1;
			flipping = 0;
			play_sound(0, 0.15, 100);
		}

		if (player.y > HEIGHT - SIZE) {
			player.x = start.x;
			player.y = start.y;
			should_flip = 0;
			gravity = 1;
			resting = 1;
			flipping = 0;
			play_sound(0, 0.15, 100);
		}

	}

	/* Flip Grav */
	if (should_flip && resting) {
		gravity *= -1;
		resting = 0;
		play_sound(1, 0.15, 100);
		should_flip = 0;
	}

	if (SDL_HasIntersection(&player, &goal)) {
		printf("You did it!\n");
		play_sound(3, 0.15, 100);

		current_level++;
		current_level %= LEVEL_COUNT;

		setup();
	}

	update_background();
}

void draw()
{
	int i;
	SDL_Rect r;

	draw_background();

	for (i = 0; i < MAXBLOCKS; i++) {
		switch (blocks[i].type) {
		case TYPE_PLATFORM:
			draw_platform(&blocks[i].r);
			break;
		case TYPE_GOAL:
			draw_goal(&blocks[i].r);
			break;
		case TYPE_START:
			draw_start(&blocks[i].r);
			break;
		}
	}

	if (gravity > 0) {
		draw_down_player(&player);
	} else {
		draw_up_player(&player);
	}

	if (edit_mode) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);
		r.x = 0;
		r.y = 0;
		r.w = 2;
		r.h = HEIGHT;
		SDL_RenderFillRect(renderer, &r);
		r.w = WIDTH;
		r.h = 2;
		SDL_RenderFillRect(renderer, &r);
		r.y = HEIGHT - 2;
		SDL_RenderFillRect(renderer, &r);
		r.x = WIDTH - 2;
		r.y = 0;
		r.w = 2;
		r.h = HEIGHT;
		SDL_RenderFillRect(renderer, &r);
	}
}

int main(int argc, char **argv)
{
	int i;

	if (argc >= 2) {
		if (strcmp(argv[1], "edit")) {
			printf("Edit mode!\n");
			edit_mode = 1;
		}
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("Unable to initialize SDL:%s ", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(
		"BBBBBB",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_OPENGL
		);

	if (window == NULL) {
		SDL_Log("Could not create window:%s \n ", SDL_GetError());
		return 1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		SDL_Log("Could not create renderer:%s \n ", SDL_GetError());
		return 1;
	}

	if (init_audio() != 0) {
		SDL_Log("Could not initialize audio:%s", SDL_GetError());
		return 1;
	}

	if (edit_mode) {
		setup_edit();
	} else {
		SDL_ShowCursor(SDL_DISABLE);
		setup();
	}

	while (1) {

		if (edit_mode) {
			if (handle_input_edit()) {
				break;
			}
		} else {

			if (handle_input()) {
				break;
			}

			current_time = SDL_GetTicks();
			if (current_time >= last_time + UPDATEMS) {
				update();
				last_time = current_time;
			}
		}


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		draw();
		SDL_RenderPresent(renderer);
	}

	clean_up_sound();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
