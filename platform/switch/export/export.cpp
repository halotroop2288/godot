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
	
	virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {
		return OK;
	}

	virtual bool can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates) const {
		String err;
		return exists_export_template("switch_debug.nro", &err) 
			&& exists_export_template("switch_release.nro", &err);
	}

	virtual List<String> get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
		List<String> list;
		list.push_back("nro");
		return list;
	}

	virtual Error export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) {

		if (!DirAccess::exists(p_path.get_base_dir())) {
			return ERR_FILE_BAD_PATH;
		}

		String template_path = p_debug ? find_export_template("switch_debug.nro")
										: find_export_template("switch_release.nro");

		if (template_path != String() && !FileAccess::exists(template_path)) {
			EditorNode::get_singleton()->show_warning(TTR("Template file not found:") + "\n" + template_path);
			return ERR_FILE_NOT_FOUND;
		}

		DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
		Error err = da->copy(template_path, p_path, 0755);
		if (err == OK) {
			String pck_path = p_path.get_basename() + ".pck";

			Vector<SharedObject> so_files;

			err = save_pack(p_preset, pck_path, &so_files);

			if (err == OK && !so_files.empty()) {
				//if shared object files, copy them
				da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
				for (int i = 0; i < so_files.size() && err == OK; i++) {
					err = da->copy(so_files[i].path, p_path.get_base_dir().plus_file(so_files[i].path.get_file()));
				}
			}
		}

		memdelete(da);
		return err;
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
