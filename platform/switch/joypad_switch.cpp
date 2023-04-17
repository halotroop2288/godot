/**************************************************************************/
/*  joypad_switch.cpp                                                     */
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

#include "joypad_switch.h"

static const SwitchJoyConfig no_controller = {};
static const SwitchJoyConfig pro_controller = {
	"Switch Pro Controller",
	HidNpadStyleTag_NpadFullKey,
	1, 1, 1, 1,
	1, 1,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Analog, SwitchTriggerStyle_Analog,
	1, 1, 1, 1, 1,
	0, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig handheld = {
	"Switch Joy-Con controllers in handheld mode",
	HidNpadStyleTag_NpadHandheld,
	1, 1, 1, 1,
	1, 1,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital,
	1, 1, 1, 1, 1,
	0, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig dual_joy = {
	"Switch Joy-Con controllers in dual mode",
	HidNpadStyleTag_NpadJoyDual,
	1, 1, 1, 1,
	1, 1,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital,
	1, 1, 1, 1, 1,
	1, 1,
	0, 0, 0, 0
};
static const SwitchJoyConfig left_joy = {
	"Switch Joy-Con left controller in single mode",
	HidNpadStyleTag_NpadJoyLeft,
	1, 1, 1, 1,
	1, 1,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital,
	1, 1, 1, 1, 1,
	1, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig right_joy = {
	"Switch Joy-Con right controller in single mode",
	HidNpadStyleTag_NpadJoyRight,
	1, 1, 1, 1,
	1, 1,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital,
	1, 1, 1, 1, 1,
	0, 1,
	0, 0, 0, 0
};
static const SwitchJoyConfig gamecube = {
	"GameCube controller",
	HidNpadStyleTag_NpadGc,
	1, 1, 1, 1,
	0, 0,
	SwitchTriggerStyle_Analog, SwitchTriggerStyle_Analog, SwitchTriggerStyle_None, SwitchTriggerStyle_Digital,
	1, 0, 1, 1, 1,
	0, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig palma = {
	"Switch Pokéball Plus controller",
	HidNpadStyleTag_NpadPalma,
	0, 0, 0, 0,
	0, 0,
	SwitchTriggerStyle_None, SwitchTriggerStyle_None, SwitchTriggerStyle_None, SwitchTriggerStyle_None,
	0, 0, 0, 0, 0,
	0, 0,
	1, 0, 0, 0
};
static const SwitchJoyConfig nes = {
	"Switch NES controller in single mode",
	HidNpadStyleTag_NpadLark,
	1, 1, 0, 0,
	0, 0,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_None, SwitchTriggerStyle_None,
	1, 1, 1, 0, 0,
	0, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig handheld_nes = {
	"Switch NES controller in handheld mode",
	HidNpadStyleTag_NpadLager,
	1, 1, 0, 0,
	0, 0,
	SwitchTriggerStyle_None, SwitchTriggerStyle_None, SwitchTriggerStyle_Digital, SwitchTriggerStyle_None,
	1, 1, 1, 0, 0,
	0, 0,
	0, 0, 1, 0
};
static const SwitchJoyConfig snes = {
	"Switch SNES controller",
	HidNpadStyleTag_NpadLucia,
	1, 1, 1, 1,
	0, 0,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_None, SwitchTriggerStyle_None,
	1, 1, 1, 0, 0,
	0, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig n64 = {
	"Switch Nintendo 64 controller",
	HidNpadStyleTag_NpadLagon,
	1, 1, 0, 0,
	0, 0,
	SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital, SwitchTriggerStyle_Digital,
	1, 1, 1, 1, 0,
	0, 0,
	0, 0, 0, 1
};
static const SwitchJoyConfig genesis = {
	"Switch SEGA Genesis controller",
	HidNpadStyleTag_NpadLager,
	1, 1, 1, 0,
	0, 0,
	SwitchTriggerStyle_None, SwitchTriggerStyle_None, SwitchTriggerStyle_Digital, SwitchTriggerStyle_None,
	1, 0, 1, 0, 0,
	0, 0,
	0, 0, 0, 0
};
static const SwitchJoyConfig joy_configurations[] = {
	pro_controller, handheld, dual_joy, left_joy, right_joy,
	gamecube, palma, nes, handheld_nes, snes, n64, genesis
};

static const u64 pad_ids[JOYPADS_MAX] = {
	(1ull << HidNpadIdType_No1) | (1ull << HidNpadIdType_Handheld),
	(1ull << HidNpadIdType_No2),
	(1ull << HidNpadIdType_No3),
	(1ull << HidNpadIdType_No4),
	(1ull << HidNpadIdType_No5),
	(1ull << HidNpadIdType_No6),
	(1ull << HidNpadIdType_No7),
	(1ull << HidNpadIdType_No8)
};

JoypadSwitch::JoypadSwitch(InputDefault *in) {
	input = in;

	// TODO: n players?
	padConfigureInput(JOYPADS_MAX, HidNpadStyleSet_NpadStandard);

	// It's (supposedly) necessary to initialize these separately as they all have different handle values
	hidGetSixAxisSensorHandles(&sixaxis_sensor_handles[0], 1, HidNpadIdType_Handheld, HidNpadStyleTag_NpadHandheld);
	hidGetSixAxisSensorHandles(&sixaxis_sensor_handles[1], 1, HidNpadIdType_No1, HidNpadStyleTag_NpadFullKey);
	hidGetSixAxisSensorHandles(&sixaxis_sensor_handles[2], 2, HidNpadIdType_No1, HidNpadStyleTag_NpadJoyDual);

	for (int i = 0; i < JOYPADS_MAX; i++) {
		auto pad = &pads[i];
		auto pad_id = (HidNpadIdType)pad_ids[i];

		padInitializeWithMask(pad, pad_id);
		hidStartSixAxisSensor(sixaxis_sensor_handles[i]);

		SwitchJoyConfig configuration = getConfiguration(pad);
		input->joy_connection_changed(i, true, configuration.name);
	}
}

JoypadSwitch::~JoypadSwitch() {
}

void JoypadSwitch::process() {
	for (int index = 0; index < JOYPADS_MAX; index++) {
		PadState *pad = &pads[index];
		padUpdate(pad);
		auto configuration = getConfiguration(pad);

		processInputs(pad, index, configuration);
		processMotion(pad, index, configuration);
	}
}

void JoypadSwitch::processInputs(PadState *pad, int index, SwitchJoyConfig configuration) {
	if (!configuration.style_tag)
		// no controller
		return;

	const u64 buttons_up = padGetButtonsUp(pad);
	const u64 buttons_down = padGetButtonsDown(pad);
	const u64 buttons_pressed = padGetButtons(pad);

	auto up = [&, buttons_up](HidNpadButton native_button) -> bool {
		return buttons_up & native_button;
	};
	auto down = [&, buttons_down](HidNpadButton native_button) -> bool {
		return buttons_down & native_button;
	};
	auto pressed = [&, buttons_pressed](HidNpadButton native_button) -> bool {
		return buttons_pressed & native_button;
	};
	auto set_godot_state = [&, index](JoystickList godot_button, bool is_pressed) -> void {
		input->joy_button(index, godot_button, is_pressed);
	};
	auto handle_button = [&, up, down](HidNpadButton native_button, JoystickList godot_button) -> void {
		if (up(native_button))
			set_godot_state(godot_button, false);
		else if (down(native_button))
			set_godot_state(godot_button, true);
	};
	auto handle_axis = [&, index](JoystickList axis, float value) -> void {
		input->joy_axis(index, axis, value);
	};

	// Axes
	if (configuration.stick_l) {
		HidAnalogStickState l_stick = padGetStickPos(pad, 0);
		handle_axis(JOY_ANALOG_LX, (float)(l_stick.x / 32767.0f));
		handle_axis(JOY_ANALOG_LY, (float)(-l_stick.y / 32767.0f));
	}
	if (configuration.stick_r) {
		HidAnalogStickState r_stick = padGetStickPos(pad, 1);
		handle_axis(JOY_ANALOG_RX, (float)(r_stick.x / 32767.0f));
		handle_axis(JOY_ANALOG_RY, (float)(-r_stick.y / 32767.0f));
	}
	if (configuration.l == SwitchTriggerStyle_Analog) {
		u32 l_trigger = padGetGcTriggerPos(pad, 0);
		handle_axis(JOY_ANALOG_L2, (float)(l_trigger / 0x7FFF));
	}
	if (configuration.r == SwitchTriggerStyle_Analog) {
		u32 r_trigger = padGetGcTriggerPos(pad, 1);
		handle_axis(JOY_ANALOG_R2, (float)(r_trigger / 0x7FFF));
	}
	if (configuration.zl == SwitchTriggerStyle_Analog) {
		u32 zl_trigger = padGetGcTriggerPos(pad, 0);
		handle_axis(JOY_ANALOG_L2, (float)(zl_trigger / 0x7FFF));
	}
	if (configuration.zr == SwitchTriggerStyle_Analog) {
		u32 zr_trigger = padGetGcTriggerPos(pad, 1);
		handle_axis(JOY_ANALOG_R2, (float)(zr_trigger / 0x7FFF));
	}

	// Buttons

	if (buttons_up || buttons_down) {
		if (configuration.a)
			handle_button(HidNpadButton_A, JOY_NINTENDO_A); // East  | Sega A
		if (configuration.b)
			handle_button(HidNpadButton_B, JOY_NINTENDO_B); // South | Sega B
		if (configuration.x)
			handle_button(HidNpadButton_X, JOY_NINTENDO_X); // North | Sega C
		if (configuration.y)
			handle_button(HidNpadButton_Y, JOY_NINTENDO_Y); // West
		if (configuration.stick_l_click)
			handle_button(HidNpadButton_StickL, JOY_L3);
		if (configuration.stick_r_click)
			handle_button(HidNpadButton_StickR, JOY_R3);
		if (configuration.l == SwitchTriggerStyle_Digital)
			handle_button(HidNpadButton_L, JOY_L);
		if (configuration.r == SwitchTriggerStyle_Digital)
			handle_button(HidNpadButton_R, JOY_R);
		if (configuration.zl == SwitchTriggerStyle_Digital)
			handle_button(HidNpadButton_ZL, JOY_L2);
		if (configuration.r == SwitchTriggerStyle_Digital)
			handle_button(HidNpadButton_R, JOY_R);
		if (configuration.plus)
			handle_button(HidNpadButton_Plus, JOY_NINTENDO_PLUS);
		if (configuration.minus)
			handle_button(HidNpadButton_Minus, JOY_NINTENDO_MINUS);
		if (configuration.d_pad) {
			handle_button(HidNpadButton_Left, JOY_DPAD_LEFT);
			handle_button(HidNpadButton_Up, JOY_DPAD_UP);
			handle_button(HidNpadButton_Right, JOY_DPAD_RIGHT);
			handle_button(HidNpadButton_Down, JOY_DPAD_DOWN);
		}
		if (configuration.pl) {
			handle_button(HidNpadButton_LeftSL, JOY_PADDLE3);
			handle_button(HidNpadButton_LeftSR, JOY_PADDLE4);
		}
		if (configuration.pr) {
			handle_button(HidNpadButton_RightSL, JOY_PADDLE1);
			handle_button(HidNpadButton_RightSR, JOY_PADDLE2);
		}
		if (configuration.palma)
			handle_button(HidNpadButton_Palma, JOY_BUTTON_0); // Pokéball button
		if (configuration.handheld_left_b)
			handle_button(HidNpadButton_HandheldLeftB, JOY_L2); // Left NES B
	}

	// Buttons-to-Axes

	auto to_axis = [&, pressed](HidNpadButton neg_button, HidNpadButton pos_button) -> float {
		float axis = 0.0f;

		if (pressed(neg_button))
			axis -= 1.0f;
		if (axis < -1.0f)
			axis = -1.0f;
		if (pressed(pos_button))
			axis += 1.0f;
		if (axis > 1.0f)
			axis = 1.0f;

		return axis;
	};

	// TODO(halotroop2288): Add digital c pad option to project settings?
	// Left+Right or Up+Down is impossible
	if (configuration.c_pad) {
		handle_axis(JOY_ANALOG_RX, to_axis(HidNpadButton_LagonCLeft, HidNpadButton_LagonCRight));
		handle_axis(JOY_ANALOG_RY, to_axis(HidNpadButton_LagonCDown, HidNpadButton_LagonCUp));
	}
}

void JoypadSwitch::processMotion(PadState *pad, int index, SwitchJoyConfig configuration) {
	if (!configuration.style_tag)
		// no controller
		return;

	// This is the actual way SwitchBrew recommends getting these values.
	HidSixAxisSensorState sixaxis = { 0 };
	if (configuration.style_tag == HidNpadStyleTag_NpadHandheld) {
		hidGetSixAxisSensorStates(sixaxis_sensor_handles[0], &sixaxis, 1);
	} else if (configuration.style_tag == HidNpadStyleTag_NpadFullKey) {
		hidGetSixAxisSensorStates(sixaxis_sensor_handles[1], &sixaxis, 1);
	} else if (configuration.style_tag == HidNpadStyleTag_NpadJoyDual) {
		u64 attrib = padGetAttributes(pad);
		if (attrib & HidNpadAttribute_IsLeftConnected)
			hidGetSixAxisSensorStates(sixaxis_sensor_handles[2], &sixaxis, 1);
		else if (attrib & HidNpadAttribute_IsRightConnected)
			hidGetSixAxisSensorStates(sixaxis_sensor_handles[3], &sixaxis, 1);
	}

	input->set_joy_accelerometer(index, Vector3(sixaxis.acceleration.x, sixaxis.acceleration.y, sixaxis.acceleration.z));
}

void JoypadSwitch::stop() {
	// Shutdown motion sensors
	for (HidSixAxisSensorHandle handle : sixaxis_sensor_handles) {
		hidStopSixAxisSensor(handle);
	}
}

SwitchJoyConfig JoypadSwitch::getConfiguration(PadState *pad) {
	if (!pad)
		return no_controller;

	u64 style_set = padGetStyleSet(pad);

	for (SwitchJoyConfig config : joy_configurations) {
		if (style_set & config.style_tag)
			return config;
	}

	return no_controller;
}
