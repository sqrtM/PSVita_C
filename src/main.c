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

typedef struct Tile
{
	char symbol;
	uint32_t color;
} Tile;

const Tile wall = {'#', 6};
const Tile floor_tile = {'.', 2};

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

int x_coord = 1;
int y_coord = 1;

void move_character()
{
	// return curser to top of screen
	printf("\e[H");
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if (i == y_coord && j == x_coord)
			{
				map[i][j].symbol = '@';
			}
			printf("%c", map[i][j].symbol);
		}
		printf("\n");
	}
	// up = 0x10, down = 0x40, left = 0x80, right = 0x20;
	printf("Buttons:%X ", ctrl.buttons);
	sceKernelDelayThread(100000);
}

int main(int argc, char *argv[])
{
	psvDebugScreenInit();
	const int font = psvDebugScreenGetFont();
	const int larger_font = psvDebugScreenScaleFont2x(font);
	psvDebugScreenSetFont(larger_font);
	// to enable analog sampling
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	move_character();
	do
	{
		sceCtrlPeekBufferPositive(0, &ctrl, 1);
		switch (ctrl.buttons)
		{
		case KEYBOARD_RIGHT:
			x_coord += 1;
			move_character();
			break;
		case KEYBOARD_DOWN:
			y_coord += 1;
			move_character();
			break;
		case KEYBOARD_UP:
			y_coord -= 1;
			move_character();
			break;
		case KEYBOARD_LEFT:
			x_coord -= 1;
			move_character();
			break;
		default:
			break;
		}
	} while (ctrl.buttons != (SCE_CTRL_START | SCE_CTRL_SELECT | SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER));

	sceKernelExitProcess(0);
	return 0;
}
