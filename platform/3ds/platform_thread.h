/**************************************************************************/
/*  thread.h                                                              */
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
#ifndef THREAD_H
#define THREAD_H

#include "core/templates/safe_refcount.h"
#include "core/typedefs.h"
#include "platform_config.h"

class String;

class Thread {
public:
	typedef void (*Callback)(void *p_userdata);

	typedef uint64_t ID;

	enum Priority {
		PRIORITY_LOW,
		PRIORITY_NORMAL,
		PRIORITY_HIGH
	};

	struct Settings {
		Priority priority;
		Settings() { priority = PRIORITY_NORMAL; }
	};

	struct PlatformFunctions {
		Error (*set_name)(const String &) = nullptr;
		void (*set_priority)(Thread::Priority) = nullptr;
		void (*init)() = nullptr;
		void (*wrapper)(Thread::Callback, void *) = nullptr;
		void (*term)() = nullptr;
	};

private:
	friend class Main;

	static ID main_thread_id;



	static uint64_t _thread_id_hash(const Handle &p_t);

	ID id = _thread_id_hash(0);
	static thread_local ID caller_id;
	CTRThread thread;

	static void callback(ID p_caller_id, const Settings &p_settings, Thread::Callback p_callback, void *p_userdata);

	static PlatformFunctions platform_functions;

public:
	enum : ID {
		UNASSIGNED_ID = 0,
		MAIN_ID = 1
	};
	static void _set_platform_functions(const PlatformFunctions &p_functions);

	_FORCE_INLINE_ ID get_id() const { return id; }
	// get the ID of the caller thread
	_FORCE_INLINE_ static ID get_caller_id() { return caller_id; }
	// get the ID of the main thread
	_FORCE_INLINE_ static ID get_main_id() { return main_thread_id; }

	static Error set_name(const String &p_name);

	void start(Thread::Callback p_callback, void *p_user, const Settings &p_settings = Settings());
	bool is_started() const;
	_FORCE_INLINE_ static bool is_main_thread() { return caller_id == MAIN_ID; }
	///< waits until thread is finished, and deallocates it.
	void wait_to_finish();
	static void make_main_thread() { caller_id = MAIN_ID; }
	static void release_main_thread() { caller_id = UNASSIGNED_ID; }
	Thread();
	~Thread();
};

#endif // THREAD_H
