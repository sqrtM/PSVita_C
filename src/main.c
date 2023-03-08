#include <vitasdk.h>
#include "include/vitasdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>

#include "../common/debugScreen.h"

#define printf psvDebugScreenPrintf

#define KEYBOARD_UP 0x10
#define KEYBOARD_DOWN 0x40
#define KEYBOARD_LEFT 0x80
#define KEYBOARD_RIGHT 0x20

SceCtrlData ctrl;

/*
TODO: put all these structs into header files
so they can can be compile time constant.

additionally, the default values for these,
like the WALL_SYMBOL should be defined at the top
so they don't have to be compile timed and they can
just be grabbed from the top.
*/
typedef struct Location
{
	int x;
	int y;
} location;

typedef struct Tile
{
	char default_symbol;
	char symbol;
	uint32_t color;
	uint32_t default_color;
} Tile;

typedef struct Player
{
	char symbol;
	location location;
} player;

const Tile wall = {'#', '#', 90, 90};
const Tile floor_tile = {'.', '.', 92, 92};

Tile map[16][16] = {
	{wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, wall, wall, wall, wall, wall, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, wall, wall},
	{wall, floor_tile, floor_tile, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, floor_tile, floor_tile, wall},
	{wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall},
};

/**
 * thank u redblobgames for the write up on how to do good, grid based
 * linear interpolation.
 * https://www.redblobgames.com/grids/line-drawing.html#interpolation-points
*/
int lerp(struct Location p0, struct Location p1)
{
	int dx = p0.x - p1.x, dy = p0.y - p1.y;
	int nx = dx < 0 ? dx * -1 : dx, ny = dy < 0 ? dy * -1 : dy;
	int sign_x = dx < 0 ? 1 : -1, sign_y = dy < 0 ? 1 : -1;

	struct Location *p;
	p = malloc(sizeof(location));
	p->x = p0.x;
	p->y = p0.y;

	int dark_tile = 0;

	for (int ix = 0, iy = 0; ix < nx || iy < ny;)
	{
		if ((0.5 + ix) / nx < (0.5 + iy) / ny)
		{
			// next step is horizontal
			p->x += sign_x;
			ix++;
		}
		else
		{
			// next step is vertical
			p->y += sign_y;
			iy++;
		}
		if (map[p->y][p->x].default_symbol == '#')
		{
			dark_tile = 1;
			break;
		}
	}
	free(p);
	return dark_tile;
}

void move_character(struct Player *player)
{

	// return curser to top of screen
	printf("\e[H");
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if (i == player->location.y && j == player->location.x)
			{
				map[i][j].symbol = player->symbol;
			}
			else
			{
				struct Location *tile_location;
				tile_location = malloc(sizeof(struct Location));
				tile_location->x = j;
				tile_location->y = i;
				int tile_dark = lerp(player->location, *tile_location);
				map[i][j].symbol = map[i][j].default_symbol;
				if (tile_dark == 1)
				{
					map[i][j].color = 2;
				}
				else
				{
					map[i][j].color = map[i][j].default_color;
				}
				free(tile_location);
			}
			printf("\e[%im%c", map[i][j].color, map[i][j].symbol);
		}
		printf("\n");
	}
	// busy wait so the player can't just
	// zip across the screen.
	sceKernelDelayThread(100000);
}

int main(int argc, char *argv[])
{

	struct Player *player;
	player = malloc(sizeof(player));
	player->symbol = '@';
	player->location.x = 1;
	player->location.y = 1;

	psvDebugScreenInit();
	PsvDebugScreenFont *font = psvDebugScreenGetFont();
	PsvDebugScreenFont *larger_font = psvDebugScreenScaleFont2x(font);
	psvDebugScreenSetFont(larger_font);
	// to enable analog sampling
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	move_character(player);
	do
	{
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		switch (ctrl.buttons)
		{
		case KEYBOARD_RIGHT:
			player->location.x += 1;
			move_character(player);
			break;
		case KEYBOARD_DOWN:
			player->location.y += 1;
			move_character(player);
			break;
		case KEYBOARD_UP:
			player->location.y -= 1;
			move_character(player);
			break;
		case KEYBOARD_LEFT:
			player->location.x -= 1;
			move_character(player);
			break;
		default:
			break;
		}
	} while (ctrl.buttons != (SCE_CTRL_START | SCE_CTRL_SELECT | SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER));

	sceKernelExitProcess(0);
	return 0;
}
