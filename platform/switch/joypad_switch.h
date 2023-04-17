/**************************************************************************/
/*  joypad_switch.h                                                       */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once
#ifndef JOYPAD_SWITCH_H
#define JOYPAD_SWITCH_H

#include "main/input_default.h"
#include "switch_wrapper.h"

#define JOYPADS_MAX 8

typedef enum {
	SwitchTriggerStyle_None = 0,
	SwitchTriggerStyle_Digital = 1,
	SwitchTriggerStyle_Analog = 2,
} SwitchTriggerStyle;

/// Describes which of the buttons should be polled for a given "Npad Style"
typedef struct {
	String name;
	HidNpadStyleTag style_tag;

	bool a;
	bool b;
	bool x; // Sega C
	bool y;
	bool stick_l_click; // Left Stick Click
	bool stick_r_click; // Right Stick Click

	SwitchTriggerStyle l;
	SwitchTriggerStyle r;
	SwitchTriggerStyle zl;
	SwitchTriggerStyle zr;

	bool plus;
	bool minus;
	bool d_pad;
	bool stick_l;
	bool stick_r;

	bool pl; // Left Dual-mode SL/SR
	bool pr; // Right Dual-mode SL/SR

	bool palma; // Pokéball Button
	bool verification;
	bool handheld_left_b; // B button on left attached NES
	bool c_pad; // N64 C-Buttons
} SwitchJoyConfig;

class JoypadSwitch {
public:
	JoypadSwitch(InputDefault *in);
	~JoypadSwitch();
	void process();
	void stop();
	SwitchJoyConfig getConfiguration(PadState *pad);

private:
	InputDefault *input;
	PadState pads[JOYPADS_MAX];
	HidSixAxisSensorHandle sixaxis_sensor_handles[JOYPADS_MAX];
	void processInputs(PadState *pad, int index, SwitchJoyConfig configuration);
	void processMotion(PadState *pad, int index, SwitchJoyConfig configuration);
};

#endif // JOYPAD_SWITCH_H
