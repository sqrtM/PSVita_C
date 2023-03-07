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
} Tile;

typedef struct Player
{
	char symbol;
	location location;
} player;

const Tile wall = {'#', '#', 0};
const Tile floor_tile = {'.', '.', 2};

Tile map[16][16] = {
	{wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, wall, wall, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, floor_tile, floor_tile, wall},
	{wall, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, floor_tile, wall, floor_tile, floor_tile, wall},
	{wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall, wall},
};

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
			} else {
				map[i][j].symbol = map[i][j].default_symbol;
			}
			printf("\e[9%im%c", map[i][j].color, map[i][j].symbol);
		}
		printf("\n");
	}
	// up = 0x10, down = 0x40, left = 0x80, right = 0x20;
	// printf("Buttons:%X ", ctrl.buttons);
	printf("Buttons:%X ", player->location.y);
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
