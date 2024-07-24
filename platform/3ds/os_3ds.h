#ifndef OS_3DS_H
#define OS_3DS_H
#include <core/os/os.h>
#include <core/input/input.h>
#include <unistd.h>

class OS_3DS : public OS {
    public:
        OS_3DS();
        void run();
        void initialize() override;
        void initialize_core();
	    void initialize_joypads() override;
        Error set_cwd(const String &p_cwd) override {
            if (chdir(p_cwd.utf8().get_data()) != 0) {
		        return ERR_CANT_OPEN;
	        }
            return OK;
        };
        String get_resource_dir() const override {
            return "romfs:/";
        }
        String get_user_data_dir() const override {
            return "sdmc:/";
        }
        virtual int get_process_exit_code(const ProcessID &p_pid) const  {
            return 0;
        }
        void set_main_loop(MainLoop *p_main_loop) override;
        virtual uint64_t get_static_memory_usage() const;
	    virtual uint64_t get_static_memory_peak_usage() const;
	    virtual uint64_t get_free_static_memory() const;
        MainLoop *get_main_loop() const override;
        void delete_main_loop() override;

        void finalize() override;
        void finalize_core() override;

        bool _check_internal_feature_support(const String &p_feature) override;

        Vector<String> get_video_adapter_driver_info() const override;
        String get_stdin_string() override;

        Error get_entropy(uint8_t *r_buffer, int p_bytes) override;

        Error execute(const String &p_path, const List<String> &p_arguments, String *r_pipe = nullptr, int *r_exitcode = nullptr, bool read_stderr = false, Mutex *p_pipe_mutex = nullptr, bool p_open_console = false) override;
        Error create_process(const String &p_path, const List<String> &p_arguments, ProcessID *r_child_id = nullptr, bool p_open_console = false) override;
        Error kill(const ProcessID &p_pid) override;
        bool is_process_running(const ProcessID &p_pid) const override;

        bool has_environment(const String &p_var) const override;
        String get_environment(const String &p_var) const override;
        void set_environment(const String &p_var, const String &p_value) const override;
        void unset_environment(const String &p_var) const override;

        String get_name() const override;
        String get_distribution_name() const override;
        String get_version() const override;

        DateTime get_datetime(bool p_utc = false) const override;
        TimeZoneInfo get_time_zone_info() const override;

        void delay_usec(uint32_t p_usec) const override;
        uint64_t get_ticks_usec() const override;

        void initialize_theme_db();
        void finalize_theme_db();
    private:
        MainLoop *main_loop = nullptr;
        SwkbdState *keyboard_state = nullptr;
        uint64_t ticks_start = 0;
};
#endif