// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include "config.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_event.h"
#include "d_main.h"
#include "i_video.h"
#include "z_zone.h"

#include "tables.h"
#include "doomkeys.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <libalarmo/lcd.h>
#include <libalarmo/input.h>

// The screen buffer; this is modified to draw things to the screen

byte *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

boolean screensaver_mode = false;

// Flag indicating whether the screen is currently visible:
// when the screen isnt visible, don't render the screen

boolean screenvisible;

// Mouse acceleration
//
// This emulates some of the behavior of DOS mouse drivers by increasing
// the speed when the mouse is moved fast.
//
// The mouse input values are input directly to the game, but when
// the values exceed the value of mouse_threshold, they are multiplied
// by mouse_acceleration to increase the speed.

float mouse_acceleration = 2.0;
int mouse_threshold = 10;

// Gamma correction level to use

int usegamma = 0;

int usemouse = 0;

// If true, keyboard mapping is ignored, like in Vanilla Doom.
// The sensible thing to do is to disable this if you have a non-US
// keyboard.

int vanilla_keyboard_mapping = true;


typedef struct
{
	byte r;
	byte g;
	byte b;
} col_t;

static col_t rgb_palette[256];

// Last states

static bool last_button_walk_state = false;
static bool last_button_fire_state = false;
static bool last_button_use_state = false;
static float last_dial_state = 0.0f;

// run state

static bool run;

// alarmo lcd buffer

static uint8_t lcd_buffer[SCREEN_HEIGHT * SCREEN_WIDTH * 3] = { 0 };

void I_InitGraphics (void)
{
	I_VideoBuffer = (byte*)Z_Malloc (SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);

	screenvisible = true;
}

void I_ShutdownGraphics (void)
{
	Z_Free (I_VideoBuffer);
}

void I_StartFrame (void)
{

}

static float getDialWheelDelta(float cur, float prev)
{
    float delta = cur - prev;

	// Try and detect wrap arounds
    if (delta > 180.0f) {
        delta -= 360.0f;
    } else if (delta < -180.0f) {
        delta += 360.0f;
    }

    return delta;
}

void I_GetEvent (void)
{
	event_t event;
	uint32_t buttons = INPUT_GetButtons();
	bool button_walk = !!(buttons & BUTTON_DIAL);
	bool button_fire = !!(buttons & BUTTON_NOTIFICATION);
	bool button_use = !!(buttons & BUTTON_BACK);

	if (button_walk != last_button_walk_state) {
		event.type = last_button_walk_state ? ev_keyup : ev_keydown;
		event.data1 = KEY_UPARROW;
		event.data2 = -1;
		event.data3 = -1;

		D_PostEvent (&event);

		last_button_walk_state = button_walk;
	}

	if (button_fire != last_button_fire_state) {
		event.type = last_button_fire_state ? ev_keyup : ev_keydown;
		event.data1 = KEY_FIRE;
		event.data2 = -1;
		event.data3 = -1;

		D_PostEvent (&event);

		event.type = last_button_fire_state ? ev_keyup : ev_keydown;
		event.data1 = KEY_ENTER;
		event.data2 = -1;
		event.data3 = -1;

		D_PostEvent (&event);

		last_button_fire_state = button_fire;
	}

	if (button_use != last_button_use_state) {
		event.type = last_button_use_state ? ev_keydown : ev_keyup;
		event.data1 = KEY_USE;
		event.data2 = -1;
		event.data3 = -1;

		D_PostEvent (&event);

		last_button_use_state = button_use;
	}

	float dial = INPUT_GetDial();
	if (dial != last_dial_state) {
		event.type = ev_mouse;
		event.data1 = 0;
		event.data2 = getDialWheelDelta(dial, last_dial_state) * -7.5f;
		event.data3 = 0;

		D_PostEvent (&event);

		last_dial_state = dial;
	}
}

void I_StartTic (void)
{
	I_GetEvent();
}

void I_UpdateNoBlit (void)
{
}

void I_FinishUpdate (void)
{
	int x, y;
	byte index;
	uint32_t offset = 0;

	// Unpalletize video buffer into lcd buffer
	for (x = SCREEN_WIDTH - 1; x >= 0; x--)
	{
		for (y = 0; y < SCREEN_HEIGHT; y++)
		{
			if (y >= SCREENHEIGHT) {
				lcd_buffer[offset++] = 0;
				lcd_buffer[offset++] = 0;
				lcd_buffer[offset++] = 0;
				continue;
			}

			index = I_VideoBuffer[y * SCREENWIDTH + x];

			lcd_buffer[offset++] = rgb_palette[index].b;
			lcd_buffer[offset++] = rgb_palette[index].g;
			lcd_buffer[offset++] = rgb_palette[index].r;
		}
	}

	LCD_DrawScreenBuffer(&lcd_buffer[0], sizeof(lcd_buffer));
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
	int i;
	col_t* c;

	for (i = 0; i < 256; i++)
	{
		c = (col_t*)palette;

		rgb_palette[i].r = gammatable[usegamma][c->r];
		rgb_palette[i].g = gammatable[usegamma][c->g];
		rgb_palette[i].b = gammatable[usegamma][c->b];

		palette += 3;
	}
}

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex (int r, int g, int b)
{
    int best, best_diff, diff;
    int i;
    col_t color;

    best = 0;
    best_diff = INT_MAX;

    for (i = 0; i < 256; ++i)
    {
    	color = rgb_palette[i];

        diff = (r - color.r) * (r - color.r)
             + (g - color.g) * (g - color.g)
             + (b - color.b) * (b - color.b);

        if (diff < best_diff)
        {
            best = i;
            best_diff = diff;
        }

        if (diff == 0)
        {
            break;
        }
    }

    return best;
}

void I_BeginRead (void)
{
}

void I_EndRead (void)
{
}

void I_SetWindowTitle (char *title)
{
}

void I_GraphicsCheckCommandLine (void)
{
}

void I_SetGrabMouseCallback (grabmouse_callback_t func)
{
}

void I_EnableLoadingDisk (void)
{
}

void I_BindVideoVariables (void)
{
}

void I_DisplayFPSDots (boolean dots_on)
{
}

void I_CheckIsScreensaver (void)
{
}
