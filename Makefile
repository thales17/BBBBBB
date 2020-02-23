CC=gcc
SDLFLAGS=`sdl2-config --cflags --libs`
FLAGS=$(SDLFLAGS) -lm
debug:
	$(CC) -o BBBBBB -g main.c $(FLAGS)
release:
	$(CC) -o BBBBBB -O2 main.c $(FLAGS)
