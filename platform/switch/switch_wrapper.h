/*************************************************************************/
/*  switch_wrapper.h                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
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

#define Mutex LibnxMutex
#define Semaphore LibnxSemaphore
#define Thread LibnxThread
#define AudioDriver LibnxAudioDriver

#define KEY_A LIBNX_KEY_A
#define KEY_B LIBNX_KEY_B
#define KEY_X LIBNX_KEY_X
#define KEY_Y LIBNX_KEY_Y
#define KEY_LSTICK LIBNX_KEY_LSTICK
#define KEY_RSTICK LIBNX_KEY_RSTICK
#define KEY_L LIBNX_KEY_L
#define KEY_R LIBNX_KEY_R
#define KEY_ZL LIBNX_KEY_ZL
#define KEY_ZR LIBNX_KEY_ZR
#define KEY_PLUS LIBNX_KEY_PLUS
#define KEY_MINUS LIBNX_KEY_MINUS
#define KEY_DLEFT LIBNX_KEY_DLEFT
#define KEY_DUP LIBNX_KEY_DUP
#define KEY_DRIGHT LIBNX_KEY_DRIGHT
#define KEY_DDOWN LIBNX_KEY_DDOWN
#define KEY_LSTICK_LEFT LIBNX_KEY_LSTICK_LEFT
#define KEY_LSTICK_UP LIBNX_KEY_LSTICK_UP
#define KEY_LSTICK_RIGHT LIBNX_KEY_LSTICK_RIGHT
#define KEY_LSTICK_DOWN LIBNX_KEY_LSTICK_DOWN
#define KEY_RSTICK_LEFT LIBNX_KEY_RSTICK_LEFT
#define KEY_RSTICK_UP LIBNX_KEY_RSTICK_UP
#define KEY_RSTICK_RIGHT LIBNX_KEY_RSTICK_RIGHT
#define KEY_RSTICK_DOWN LIBNX_KEY_RSTICK_DOWN
#define KEY_SL_LEFT LIBNX_KEY_SL_LEFT
#define KEY_SR_LEFT LIBNX_KEY_SR_LEFT
#define KEY_SL_RIGHT LIBNX_KEY_SL_RIGHT
#define KEY_SR_RIGHT LIBNX_KEY_SR_RIGHT
#define KEY_HOME LIBNX_KEY_HOME
#define KEY_CAPTURE LIBNX_KEY_CAPTURE
#define KEY_TOUCH LIBNX_KEY_TOUCH
#define KEY_JOYCON_RIGHT LIBNX_KEY_JOYCON_RIGHT
#define KEY_JOYCON_DOWN LIBNX_KEY_JOYCON_DOWN
#define KEY_JOYCON_UP LIBNX_KEY_JOYCON_UP
#define KEY_JOYCON_LEFT LIBNX_KEY_JOYCON_LEFT
#define KEY_UP LIBNX_KEY_UP
#define KEY_DOWN LIBNX_KEY_DOWN
#define KEY_LEFT LIBNX_KEY_LEFT
#define KEY_RIGHT LIBNX_KEY_RIGHT
#define KEY_SL LIBNX_KEY_SL
#define KEY_SR LIBNX_KEY_SR

#include <switch.h>
#undef AudioDriver
#undef Thread
#undef Mutex
#undef Semaphore

#undef KEY_A
#undef KEY_B
#undef KEY_X
#undef KEY_Y
#undef KEY_LSTICK
#undef KEY_RSTICK
#undef KEY_L
#undef KEY_R
#undef KEY_ZL
#undef KEY_ZR
#undef KEY_PLUS
#undef KEY_MINUS
#undef KEY_DLEFT
#undef KEY_DUP
#undef KEY_DRIGHT
#undef KEY_DDOWN
#undef KEY_LSTICK_LEFT
#undef KEY_LSTICK_UP
#undef KEY_LSTICK_RIGHT
#undef KEY_LSTICK_DOWN
#undef KEY_RSTICK_LEFT
#undef KEY_RSTICK_UP
#undef KEY_RSTICK_RIGHT
#undef KEY_RSTICK_DOWN
#undef KEY_SL_LEFT
#undef KEY_SR_LEFT
#undef KEY_SL_RIGHT
#undef KEY_SR_RIGHT
#undef KEY_HOME
#undef KEY_CAPTURE
#undef KEY_TOUCH
#undef KEY_JOYCON_RIGHT
#undef KEY_JOYCON_DOWN
#undef KEY_JOYCON_UP
#undef KEY_JOYCON_LEFT
#undef KEY_UP
#undef KEY_DOWN
#undef KEY_LEFT
#undef KEY_RIGHT
#undef KEY_SL
#undef KEY_SR
