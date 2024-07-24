#include "platform_config.h"
#include "os_3ds.h"
#include <main/main.h>
#include "servers/display_server.h"
#include "servers/display_server_headless.h"
#include "servers/rendering_server.h"
#include "filesystem/file_access_3ds.h"
#include "filesystem/dir_access_3ds.h"
#include "servers/display/display_server_3ds.h"
#include <time.h>

static aptHookCookie apt_hook_cookie;
static void apt_hook_callback(APT_HookType hook, void* param)
{
	if (hook == APTHOOK_ONRESTORE || hook == APTHOOK_ONWAKEUP) {
		
	}
}

OS_3DS::OS_3DS(){

    aptHook(&apt_hook_cookie, apt_hook_callback, this);
}

void OS_3DS::initialize_core() {
    osSetSpeedupEnable(true);
    psInit();
    archiveMountSdmc();
    romfsInit();
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

    ticks_start = svcGetSystemTick();
    FileAccess::make_default<FileAccess3DS>(FileAccess::ACCESS_RESOURCES);
	FileAccess::make_default<FileAccess3DS>(FileAccess::ACCESS_USERDATA);
	FileAccess::make_default<FileAccess3DS>(FileAccess::ACCESS_FILESYSTEM);

    DirAccess::make_default<DirAccess3DS>(DirAccess::ACCESS_RESOURCES);
	DirAccess::make_default<DirAccess3DS>(DirAccess::ACCESS_USERDATA);
	DirAccess::make_default<DirAccess3DS>(DirAccess::ACCESS_FILESYSTEM);

    DisplayServer3DS::register_3ds_driver();
}   

void OS_3DS::finalize() {
    psExit();
    romfsExit();
    archiveUnmountAll();
}

void OS_3DS::finalize_core() {

}

void OS_3DS::initialize() {
    main_loop = nullptr;
    initialize_core();
    set_current_rendering_driver_name("3ds");
    set_current_rendering_method("3ds");
    set_display_driver_id(0);
}

void OS_3DS::initialize_joypads() {

}

void OS_3DS::run() {

    if (!main_loop)
    {
        return;
    }

    main_loop->initialize();
    while (!Main::iteration())
    {
        DisplayServer::get_singleton()->process_events();
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            SceneTree::get_singleton()->quit(EXIT_SUCCESS);
    }
    
    main_loop->finalize();
}


void OS_3DS::set_main_loop(MainLoop *p_main_loop){
    main_loop = p_main_loop;
}

MainLoop *OS_3DS::get_main_loop() const {
    return main_loop;
}

void OS_3DS::delete_main_loop() {
    if (main_loop)
    {
        memdelete(main_loop);
    }
    main_loop = nullptr;
}

uint64_t OS_3DS::get_static_memory_usage() const {
    return get_static_memory_peak_usage() - get_free_static_memory();
}
uint64_t OS_3DS::get_static_memory_peak_usage() const{
    return osGetMemRegionSize(MEMREGION_APPLICATION);
}
uint64_t OS_3DS::get_free_static_memory() const{
    return osGetMemRegionFree(MEMREGION_APPLICATION);
}

bool OS_3DS::_check_internal_feature_support(const String &p_feature) {
    return false;
}

Vector<String> OS_3DS::get_video_adapter_driver_info() const {
    if (RenderingServer::get_singleton()->get_rendering_device() == nullptr) {
		return Vector<String>();
	}
    Vector<String> vec = Vector<String>();
    vec.push_back(String("3ds"));
    return vec;
}

String OS_3DS::get_stdin_string() {
    char buff[1024];
	return String::utf8(fgets(buff, 1024, stdin));
}

Error OS_3DS::get_entropy(uint8_t *r_buffer, int p_bytes) {
    Result result = PS_GenerateRandomBytes(r_buffer,p_bytes);
    ERR_FAIL_COND_V(R_FAILED(result),FAILED);
    return OK;
}

Error OS_3DS::execute(const String &p_path, const List<String> &p_arguments, String *r_pipe, int *r_exitcode, bool read_stderr, Mutex *p_pipe_mutex, bool p_open_console)
{
    return FAILED;
}

Error OS_3DS::create_process(const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id, bool p_open_console)
{
    return FAILED;
}

Error OS_3DS::kill(const ProcessID &p_pid)
{
    return FAILED;
}

bool OS_3DS::is_process_running(const ProcessID &p_pid) const {
    return false;
}

bool OS_3DS::has_environment(const String &p_var) const {
    return false;
}

String OS_3DS::get_environment(const String &p_var) const {
    return "";
}

void OS_3DS::set_environment(const String &p_var, const String &p_value) const {

}

void OS_3DS::unset_environment(const String &p_var) const {

}

String OS_3DS::get_name() const {
    return "Nintendo 3DS";
}

String OS_3DS::get_distribution_name() const {
    return "Horizon";
}

String OS_3DS::get_version() const {
    char buffer[256];
    Result res = osGetSystemVersionDataString(NULL,NULL,buffer,256);
    if (R_FAILED(res))
    {
        return "";
    }
    return buffer;
}

OS::DateTime OS_3DS::get_datetime(bool p_utc) const {
    time_t t = time(nullptr);
	struct tm lt;
	if (p_utc) {
		gmtime_r(&t, &lt);
	} else {
		localtime_r(&t, &lt);
	}
	DateTime ret;
	ret.year = 1900 + lt.tm_year;
	// Index starting at 1 to match OS_Unix::get_date
	//   and Windows SYSTEMTIME and tm_mon follows the typical structure
	//   of 0-11, noted here: http://www.cplusplus.com/reference/ctime/tm/
	ret.month = (Month)(lt.tm_mon + 1);
	ret.day = lt.tm_mday;
	ret.weekday = (Weekday)lt.tm_wday;
	ret.hour = lt.tm_hour;
	ret.minute = lt.tm_min;
	ret.second = lt.tm_sec;
	ret.dst = lt.tm_isdst;
	return ret;
}

OS::TimeZoneInfo OS_3DS::get_time_zone_info() const {
    time_t t = time(nullptr);
	struct tm lt;
	localtime_r(&t, &lt);
	char name[16];
	strftime(name, 16, "%Z", &lt);
	name[15] = 0;
	TimeZoneInfo ret;
	ret.name = name;
	char bias_buf[16];
	strftime(bias_buf, 16, "%z", &lt);
	int bias;
	bias_buf[15] = 0;
	sscanf(bias_buf, "%d", &bias);
	// convert from ISO 8601 (1 minute=1, 1 hour=100) to minutes
	int hour = (int)bias / 100;
	int minutes = bias % 100;
	if (bias < 0) {
		ret.bias = hour * 60 - minutes;
	} else {
		ret.bias = hour * 60 + minutes;
	}
	return ret;
}

void OS_3DS::delay_usec(uint32_t p_usec) const {
    svcSleepThread(1000ULL * p_usec);
}
#define TICKS_PER_SEC 268123480ULL
#define TICKS_PER_USEC 268
uint64_t OS_3DS::get_ticks_usec() const{
   uint64_t ticks =  (svcGetSystemTick() - ticks_start) / TICKS_PER_USEC;
   return ticks;
}