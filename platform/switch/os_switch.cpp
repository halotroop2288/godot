#include <switch.h>
#include "os_switch.h"
#include <stdio.h>

void OS_Switch::initialize_core() {}

Error OS_Switch::initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver)
{
	return FAILED;
}

void OS_Switch::set_main_loop(MainLoop *p_main_loop) {}
void OS_Switch::delete_main_loop() {}
void OS_Switch::finalize() {}
void OS_Switch::finalize_core() {}

bool OS_Switch::_check_internal_feature_support(const String &p_feature) { return false; }

void OS_Switch::alert(const String &p_alert, const String &p_title) {}
String OS_Switch::get_stdin_string(bool p_block) { return ""; }
Point2 OS_Switch::get_mouse_position() const
{
	return Point2(0, 0);
}

int OS_Switch::get_mouse_button_state() const
{
	return 0;
}

void OS_Switch::set_window_title(const String &p_title) {}

void OS_Switch::set_video_mode(const OS::VideoMode &p_video_mode, int p_screen) {}
OS::VideoMode OS_Switch::get_video_mode(int p_screen) const
{
	return VideoMode(0, 0);
}

void OS_Switch::get_fullscreen_mode_list(List<OS::VideoMode> *p_list, int p_screen) const {}

int OS_Switch::get_current_video_driver() const { return 0; }
Size2 OS_Switch::get_window_size() const { return Size2(0,0); }

Error OS_Switch::execute(const String &p_path, const List<String> &p_arguments, bool p_blocking, ProcessID *r_child_id, String *r_pipe, int *r_exitcode, bool read_stderr)
{
	return FAILED;
}

Error OS_Switch::kill(const ProcessID &p_pid)
{
	return FAILED;
}

bool OS_Switch::has_environment(const String &p_var) const
{
	return false;
}

String OS_Switch::get_environment(const String &p_var) const
{
	return "";
}

String OS_Switch::get_name()
{
	return "Switch";
}

MainLoop * OS_Switch::get_main_loop() const
{
	return nullptr;
}

OS::Date OS_Switch::get_date(bool local) const
{
	return OS::Date();
}

OS::Time OS_Switch::get_time(bool local) const
{
	return OS::Time();
}

OS::TimeZoneInfo OS_Switch::get_time_zone_info() const
{
	return OS::TimeZoneInfo();
}

void OS_Switch::delay_usec(uint32_t p_usec) const {}

uint64_t OS_Switch::get_ticks_usec() const
{
	return 0;
}

bool OS_Switch::can_draw() const
{
	return false;
}

void OS_Switch::set_cursor_shape(CursorShape p_shape) {}
void OS_Switch::set_custom_mouse_cursor(const RES &p_cursor, CursorShape p_shape, const Vector2 &p_hotspot) {}

void OS_Switch::run()
{
	gfxInitDefault();
    consoleInit(NULL);

	printf("haha butts\n");
	while(appletMainLoop())
    {
    	hidScanInput();
    	u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
    	if (kDown & KEY_PLUS) break; // break in order to return to hbmenu
    	gfxFlushBuffers();
        gfxSwapBuffers();
    }
    gfxExit();
}