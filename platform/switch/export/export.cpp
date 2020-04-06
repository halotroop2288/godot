/*************************************************************************/
/*  export.cpp                                                           */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "export.h"

#include "core/os/file_access.h"
#include "editor/editor_export.h"
#include "editor/editor_node.h"
#include "platform/switch/logo.gen.h"
#include "scene/resources/texture.h"
#include "core/io/packet_peer_udp.h"

#define TEMPLATE_RELEASE "switch_release.nro"

class EditorExportPlatformSwitch : public EditorExportPlatform {

	GDCLASS(EditorExportPlatformSwitch, EditorExportPlatform)

	Ref<ImageTexture> logo;

	Vector<String> devices;
	volatile bool devices_changed;
	Mutex *device_lock;
	Thread *device_thread;
	volatile bool quit_request;

	static void _device_poll_thread(void *ud) {
		EditorExportPlatformSwitch *ea = (EditorExportPlatformSwitch *)ud;

		PacketPeerUDP peer;
		if(peer.listen(28771) != OK)
		{
			// ???
		}
		
		peer.set_broadcast_enabled(true);
		peer.set_dest_address(IP_Address("255.255.255.255"), 28280);

		const uint8_t *packet_buff;
		int packet_len;

		while(!ea->quit_request) {
			bool different = false;

			peer.put_packet((unsigned char*)"nxboot", strlen("nxboot"));
			OS::get_singleton()->delay_usec(500000); // delay 500ms to allow for actual replies
			
			Vector<String> ndevices;	

			while(peer.get_packet(&packet_buff, packet_len) != ERR_UNAVAILABLE)
			{
				IP_Address a = peer.get_packet_address();
				ndevices.push_back(String(a));
			}
			
			ndevices.sort();

			ea->device_lock->lock();

			if(ndevices.size() != ea->devices.size()) 
			{
				different = true;
			}
			else
			{
				for (int i = 0; i < ea->devices.size(); i++)
				{
					if (ea->devices[i] != ndevices[i])
					{
						different = true;
						break;
					}
				}
			}

			if(different)
			{
				ea->devices = ndevices;
				ea->devices_changed = true;
			}

			ea->device_lock->unlock();

			uint64_t sleep = OS::get_singleton()->get_power_state() == OS::POWERSTATE_ON_BATTERY ? 1000 : 100;
			uint64_t wait = 3000000;
			uint64_t time = OS::get_singleton()->get_ticks_usec();
			while (OS::get_singleton()->get_ticks_usec() - time < wait) {
				OS::get_singleton()->delay_usec(1000 * sleep);
				if (ea->quit_request)
					break;
			}
		}
	}

public:

	virtual void get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features) {
		String driver = ProjectSettings::get_singleton()->get("rendering/quality/driver/driver_name");
		if (driver == "GLES2") {
			r_features->push_back("etc");
		} else if (driver == "GLES3") {
			r_features->push_back("etc2");
			if (ProjectSettings::get_singleton()->get("rendering/quality/driver/fallback_to_gles2")) {
				r_features->push_back("etc");
			}
		}
	}

	virtual void get_platform_features(List<String> *r_features) {
		r_features->push_back("mobile");
	}

	virtual void get_export_options(List<ExportOption> *r_options) {
		String title = ProjectSettings::get_singleton()->get("application/config/name");
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/title", PROPERTY_HINT_PLACEHOLDER_TEXT, title), title));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/author", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Author"), "Stary & Cpasjuste"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/version", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Version"), "1.0"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "application/icon_256x256", PROPERTY_HINT_GLOBAL_FILE, "*.jpg"), ""));
	}
	
	virtual String get_name() const {
		return "Switch";
	}

	virtual String get_os_name() const {
		return "Switch";
	}

	virtual Ref<Texture> get_logo() const {
		return logo;
	}

	virtual Ref<Texture> get_run_icon() const {
		return logo;
	}

	virtual bool poll_export() {
		bool dc = devices_changed;
		if (dc) {
			// don't clear unless we're reporting true, to avoid race
			devices_changed = false;
		}
		return dc;
	}

	virtual int get_options_count() const {
		device_lock->lock();
		int dc = devices.size();
		device_lock->unlock();

		return dc;
	}

	virtual String get_options_tooltip() const {
		return TTR("Select device from the list");
	}

	virtual String get_option_label(int p_index) const {
		ERR_FAIL_INDEX_V(p_index, devices.size(), "");
		device_lock->lock();
		String s = devices[p_index];
		device_lock->unlock();
		return s;
	}

	virtual String get_option_tooltip(int p_index) const {
		ERR_FAIL_INDEX_V(p_index, devices.size(), "");
		device_lock->lock();
		String s = devices[p_index];
		device_lock->unlock();
		return s;
	}
	virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {
		String can_export_error;
		bool can_export_missing_templates;
		if (!can_export(p_preset, can_export_error, can_export_missing_templates)) {
			EditorNode::add_io_error(can_export_error);
			return ERR_UNCONFIGURED;
		}

		EditorProgress ep("run", "Running", 2);

		print_line("Exporting...");
		if (ep.step("Exporting...", 0)) {
			return ERR_SKIP;
		}

		String tmp_export_path = EditorSettings::get_singleton()->get_cache_dir().plus_file("tmpexport.pck");
		Error err = export_project(p_preset, true, tmp_export_path, p_debug_flags);

		if (err != OK) {
			DirAccess::remove_file_or_error(tmp_export_path);
			return err;
		}

		print_line("Sending...");
		if (ep.step("Sending...", 1)) {
			DirAccess::remove_file_or_error(tmp_export_path);
			return err;
		}

		String nxlink = EditorSettings::get_singleton()->get("export/switch/nxlink");
		if (FileAccess::exists(nxlink)) {
			List<String> args;
			int ec;

			args.push_back(tmp_export_path);
			args.push_back("-a");
			args.push_back(devices[p_device]);
			args.push_back("-p");
			args.push_back("TempExport.pck");
			args.push_back("--args");

			Vector<String> inner_args;
			// todo: editor arg
			inner_args.push_back("-v");

			// TODO: hack!!! fix this lmao
			inner_args.push_back("--main-pack");
			inner_args.push_back("sdmc:/switch/TempExport.pck");

			gen_export_flags(inner_args, p_debug_flags);
			args.push_back(String(" ").join(inner_args));

			OS::get_singleton()->execute(nxlink, args, true, NULL, NULL, &ec);
		}

		DirAccess::remove_file_or_error(tmp_export_path);
		return OK;
	}

	virtual bool can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates) const {

		String err;
		r_missing_templates = find_export_template(TEMPLATE_RELEASE) == String();

		bool valid = !r_missing_templates;
		String etc_error = test_etc2();
		if (etc_error != String()) {
			err += etc_error;
			valid = false;
		}

		if (!err.empty()) {
			r_error = err;
		}

		return valid;
	}

	virtual List<String> get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
		List<String> list;
		list.push_back("pck");
		return list;
	}

	virtual void resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, Set<String> &p_features) {
	}

	virtual Error export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) {
		if (!DirAccess::exists(p_path.get_base_dir())) {
			return ERR_FILE_BAD_PATH;
		}
		String template_path = find_export_template(TEMPLATE_RELEASE);

		if (template_path != String() && !FileAccess::exists(template_path)) {
			EditorNode::get_singleton()->show_warning(TTR("Template file not found:") + "\n" + template_path);
			return ERR_FILE_NOT_FOUND;
		}
		DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
		Error err = da->copy(template_path, p_path, 0755);
		if (err == OK) {
			// update nro icon/title/author/version
			String title = p_preset->get("application/title");
			String author = p_preset->get("application/author");
			String version = p_preset->get("application/version");
			String icon = p_preset->get("application/icon_256x256");
			if(icon != "" && DirAccess::exists(icon))
			{
				err = da->copy(icon, p_path.get_basename() + ".jpg", 0755);
				if(err != OK) {
					return ERR_BUG;
				}
			}

			String nacp_path = p_path.get_basename() + ".nacp";
			err = create_nacp(nacp_path, title, author, version);
			if(err != OK) {
				return ERR_BUG;
			}
				
			String pck_path = p_path.get_basename() + ".pck";
			err = save_pack(p_preset, pck_path);
		}

		memdelete(da);
		return err;
	}

	Error create_nacp(String &nacp_path, String& title, String& author, String& version) {
		const char *title_cstr = title.utf8().ptr();
		const char *author_cstr = author.utf8().ptr();
		const char *version_cstr = version.utf8().ptr();

		NacpStruct *nacp = (NacpStruct *)malloc(sizeof(NacpStruct));
		memset(nacp, 0, sizeof(NacpStruct));

		for (int i = 0; i < 12; i++) {
			strncpy(nacp->lang[i].name, title_cstr, sizeof(nacp->lang[i].name) - 1);
			strncpy(nacp->lang[i].author, author_cstr, sizeof(nacp->lang[i].author) - 1);
		}
		strncpy(nacp->version, version_cstr, sizeof(nacp->version) - 1);
		
		FileAccess *nacp_file = FileAccess::open(nacp_path, FileAccess::WRITE);
		nacp_file->store_buffer((const uint8_t*)nacp, sizeof(NacpStruct));
		nacp_file->close();
		free(nacp);

		return OK;
	}

	EditorExportPlatformSwitch() {
		Ref<Image> img = memnew(Image(_switch_logo));
		logo.instance();
		logo->create_from_image(img);

		device_lock = Mutex::create();
		devices_changed = true;
		quit_request = false;
		device_thread = Thread::create(_device_poll_thread, this);
	}
	
	~EditorExportPlatformSwitch() {
	}
};


void register_switch_exporter() {
	String exe_ext;
	if (OS::get_singleton()->get_name() == "Windows") {
		exe_ext = "*.exe";
	}

	EDITOR_DEF("export/switch/nxlink", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/switch/nxlink", PROPERTY_HINT_GLOBAL_FILE, exe_ext));

	Ref<EditorExportPlatformSwitch> exporter = Ref<EditorExportPlatformSwitch>(memnew(EditorExportPlatformSwitch));
	EditorExport::get_singleton()->add_export_platform(exporter);
}
