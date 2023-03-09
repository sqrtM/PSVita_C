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
like the w_t_SYMBOL should be defined at the top
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
	int seen;
	uint32_t color;
	uint32_t default_color;
} Tile;

typedef struct Player
{
	char symbol;
	location location;
} player;

const Tile w_t = {'#', '#', 0, 90, 90};
const Tile f_t = {'.', '.', 0, 92, 92};

Tile map[28][48] = {
	{w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, f_t, f_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, f_t, f_t, f_t, f_t, f_t, f_t, f_t, w_t, w_t, w_t},
	{w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t, w_t},
};

/**
 * thank u redblobgames for the write up on how to do good, grid based
 * linear interpolation.
 * https://www.redblobgames.com/grids/line-drawing.html#interpolation-points
 */
int lerp(struct Location p0, struct Location p1)
{
	// get differences
	int dx = p0.x - p1.x, dy = p0.y - p1.y;
	// get the abs of those differences
	int nx = dx < 0 ? dx * -1 : dx, ny = dy < 0 ? dy * -1 : dy;
	// decide the direction to trace the line
	int sign_x = dx < 0 ? 1 : -1, sign_y = dy < 0 ? 1 : -1;

	struct Location *p;
	p = malloc(sizeof(location));
	p->x = p0.x;
	p->y = p0.y;

	int dark_tile = 0;

	for (int ix = 0, iy = 0; ix < nx || iy < ny;)
	{
		// if x is less than y...
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
			map[p->y][p->x].seen = 1;
			dark_tile = 1;
			break;
		}
	}
	free(p);
	return dark_tile;
}

// next thing TODO ...
void randomize_map(Tile arr[28][48])
{
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 48; j++)
		{
			arr[i][j] = rand() % 2 == 0 ? f_t : w_t;
		}
	}
}

void move_character(struct Player *player)
{

	// return curser to top of screen
	printf("\e[H");
	for (int i = 0; i < 28; i++)
	{
		for (int j = 0; j < 48; j++)
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
				int tile_behind_w_t = lerp(player->location, *tile_location);
				map[i][j].symbol = map[i][j].default_symbol;

				// if it has not been seen
				if (map[i][j].seen == 0)
				{
					// and it is behind a w_t
					if (tile_behind_w_t == 1)
					{
						// it is drawn black
						map[i][j].color = 30;
					}
					else
					{
						// we mark it as seen
						// and paint it bright green
						map[i][j].seen = 1;
						map[i][j].color = 92;
					}
				}
				// if we HAVE already seen it.
				else
				{
					// if it is behind a w_t
					if (tile_behind_w_t == 1)
					{
						// we paint it dark green
						map[i][j].color = 32;
					}
					else
					{
						// we paint it bright green
						map[i][j].color = 92;
					}
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

	// randomize_map(map);
	move_character(player);
	do
	{
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		switch (ctrl.buttons)
		{
		case KEYBOARD_RIGHT:
			if (map[player->location.y][player->location.x + 1].default_symbol == '#')
			{
				break;
			}
			player->location.x += 1;
			move_character(player);
			break;
		case KEYBOARD_DOWN:
			if (map[player->location.y + 1][player->location.x].default_symbol == '#')
			{
				break;
			}
			player->location.y += 1;
			move_character(player);
			break;
		case KEYBOARD_UP:
			if (map[player->location.y - 1][player->location.x].default_symbol == '#')
			{
				break;
			}
			player->location.y -= 1;
			move_character(player);
			break;
		case KEYBOARD_LEFT:
			if (map[player->location.y][player->location.x - 1].default_symbol == '#')
			{
				break;
			}
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
