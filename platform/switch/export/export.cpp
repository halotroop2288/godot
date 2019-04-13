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

#include "editor/editor_export.h"
#include "platform/switch/logo.gen.h"
#include "scene/resources/texture.h"

class EditorExportPlatformSwitch : public EditorExportPlatform {

	GDCLASS(EditorExportPlatformSwitch, EditorExportPlatform)

	Ref<ImageTexture> logo;
	
public:

	virtual void get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features) {}

	virtual void get_export_options(List<ExportOption> *r_options) {}
	
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
	
	virtual bool poll_devices() {
		return false;
	}

	virtual int get_device_count() const {
		return 2;
	}

	virtual String get_device_name(int p_device) const {
		if(p_device) {
			return "smdc:";
		} else {
			return "romfs:";
		}
	}
	
	virtual String get_device_info(int p_device) const {
		if(p_device) {
			return "smdc:";
		} else {
			return "romfs:";
		}
	}
	
	virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {
		return OK;
	}

	virtual bool can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates) const {
		return true;
	}

	virtual List<String> get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
		List<String> list;
		list.push_back("nro");
		return list;
	}

	virtual Error export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) {
		return OK;
	}

	virtual void get_platform_features(List<String> *r_features) {
	}
	
	virtual void resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, Set<String> &p_features) {
	}

	EditorExportPlatformSwitch() {
		
		Ref<Image> img = memnew(Image(_switch_logo));
		logo.instance();
		logo->create_from_image(img);
	}
	
	~EditorExportPlatformSwitch() {
	}
};

void register_switch_exporter() {
	
	Ref<EditorExportPlatformSwitch> exporter = Ref<EditorExportPlatformSwitch>(memnew(EditorExportPlatformSwitch));
	EditorExport::get_singleton()->add_export_platform(exporter);
}
