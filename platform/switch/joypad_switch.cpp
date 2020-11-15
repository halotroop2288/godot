/*************************************************************************/
/*  joypad_switch.cpp                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "joypad_switch.h"

static HidControllerID pad_id[JOYPADS_MAX] = {
	CONTROLLER_P1_AUTO, CONTROLLER_PLAYER_2,
	CONTROLLER_PLAYER_3, CONTROLLER_PLAYER_4,
	CONTROLLER_PLAYER_5, CONTROLLER_PLAYER_6,
	CONTROLLER_PLAYER_7, CONTROLLER_PLAYER_8
};

// from editor "Project Settings > Input Map"
static const HidControllerKeys pad_mapping[] = {
	LIBNX_KEY_B, LIBNX_KEY_A, LIBNX_KEY_Y, LIBNX_KEY_X,
	LIBNX_KEY_L, LIBNX_KEY_R, LIBNX_KEY_ZL, LIBNX_KEY_ZR,
	LIBNX_KEY_LSTICK, LIBNX_KEY_RSTICK,
	LIBNX_KEY_MINUS, LIBNX_KEY_PLUS,
	LIBNX_KEY_DUP, LIBNX_KEY_DDOWN, LIBNX_KEY_DLEFT, LIBNX_KEY_DRIGHT
};

JoypadSwitch::JoypadSwitch(InputDefault *in) {
	input = in;
	button_count = sizeof(pad_mapping) / sizeof(*pad_mapping);
	for (int i = 0; i < JOYPADS_MAX; i++) {
		pads[i].id = pad_id[i];
	}
}

JoypadSwitch::~JoypadSwitch() {
}

void JoypadSwitch::process() {

	u64 changed;
	static JoypadState pad_old[JOYPADS_MAX];

	for (int index = 0; index < JOYPADS_MAX; index++) {

		hidJoystickRead(&pads[index].l_pos, pads[index].id, JOYSTICK_LEFT);
		hidJoystickRead(&pads[index].r_pos, pads[index].id, JOYSTICK_RIGHT);
		pads[index].buttons = hidKeysHeld(pads[index].id);

		// Axes
		input->joy_axis(index, 0, { 100, (float)(pads[index].l_pos.dx / 32767.0f) });
		input->joy_axis(index, 1, { 100, (float)(-pads[index].l_pos.dy / 32767.0f) });
		input->joy_axis(index, 2, { 100, (float)(pads[index].r_pos.dx / 32767.0f) });
		input->joy_axis(index, 3, { 100, (float)(-pads[index].r_pos.dy / 32767.0f) });

		// Buttons
		changed = pad_old[index].buttons ^ pads[index].buttons;
		pad_old[index].buttons = pads[index].buttons;
		if (changed) {
			for (int i = 0; i < button_count; i++) {
				if (changed & pad_mapping[i]) {
					input->joy_button(index, i, (bool)(pads[index].buttons & pad_mapping[i]));
				}
			}
		}
	}
}
