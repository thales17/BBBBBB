# BBBBBB

A small game inspired by VVVVVV.

![BBBBBB gameplay gif](BBBBBB.gif)

## Building

### Dependencies
To build this game you will need the following:
- gcc / clang
- SDL2
- make

### Building
- run `make release` from the project root to build the game
- The Makefile will produce a binary named `BBBBBB`

## Playing the game
- Move the character around with either the arrow keys or the WASD keys
- Press the space bar to flip gravity
- The goal of the game is to reach the goal diamond and move on to the next level
- There are 7 levels, after beating the 7th the game repeats

## Edit Mode
- You can easily create you own levels by using edit mode
- To enter into edit mode press `CTRL-e`
- You should see a yellow border around the edges of the game and the player movement should stop
- To modify level hold one of the mod keys (B, G, P) and use the left mouse button
- B = Block or Platform, G = Goal, P = Player Starting point
- Press the space bar to output the level grid data to `stdout`
- You can then copy and paste this data into the [level.h](level.h) file. * Don't forget to update the level count
