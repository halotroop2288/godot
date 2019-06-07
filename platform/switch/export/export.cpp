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
#include "platform/switch/icon_default.h"
#include "scene/resources/texture.h"

#define TEMPLATE_RELEASE "switch_release.nro"

class EditorExportPlatformSwitch : public EditorExportPlatform {

	GDCLASS(EditorExportPlatformSwitch, EditorExportPlatform)

	Ref<ImageTexture> logo;
	
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
	
	virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {
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
		list.push_back("nro");
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

			err = update_nro(p_path.ascii().ptr(), icon.ascii().ptr(), title.ascii().ptr(), author.ascii().ptr(), version.ascii().ptr());
			if(err != OK) {
				return ERR_BUG;
			}

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

	Error update_nro(const char *nro_path, const char *icon_path, const char *title, const char *author, const char *version) {

		NroHeader *nro_hdr;
		AssetHeader *asset_hdr;
		NacpStruct *nacp;

		printf("\nupdate_nro: %s, %s, %s, %s, %s\n", nro_path, icon_path, title, author, version);

		// read nro header to memory
		printf("NRO: reading nro header, %li bytes @ %li\n", sizeof(NroHeader), sizeof(NroStart));
		nro_hdr = reinterpret_cast<NroHeader *>(read_bytes(nro_path, sizeof(NroStart), sizeof(NroHeader)));
		if (nro_hdr == nullptr) {
			printf("error: could not read nro header\n");
			return ERR_INVALID_DATA;
		}
		// is nro header valid?
		printf("NRO: checking nro header magic\n");
		if (strncmp((char *) nro_hdr->Magic, "NRO0", 4) != 0) {
			printf("error: nro magic not found\n");
			free(nro_hdr);
			return ERR_INVALID_DATA;
		}
		// nro header read success
		printf("NRO: magic: %s, asset header offset: %i\n", nro_hdr->Magic, nro_hdr->size);

		// read asset header to memory
		printf("ASSET: reading asset header, %li bytes @ %i\n", sizeof(AssetHeader), nro_hdr->size);
		asset_hdr = reinterpret_cast<AssetHeader *>(read_bytes(nro_path, nro_hdr->size, sizeof(AssetHeader)));
		if (asset_hdr == nullptr) {
			printf("error: could not read asset header\n");
			free(nro_hdr);
			return ERR_INVALID_DATA;
		}
		// is asset header valid?
		printf("ASSET: checking asset header magic\n");
		if (strncmp((char *) asset_hdr->magic, "ASET", 4) != 0) {
			printf("error: asset magic not found\n");
			free(asset_hdr);
			free(nro_hdr);
			return ERR_INVALID_DATA;
		}
		// asset header read success
		printf("ASSET: magic: %s, version: %i, icon offset: %li, icon size: %li\n",
			   asset_hdr->magic, asset_hdr->version, asset_hdr->icon.offset, asset_hdr->icon.size);

		// update icon
		int icon = 1;
		size_t wrote = 0;
		size_t icon_size = 0;
		size_t icon_offset = nro_hdr->size + asset_hdr->icon.offset;
		printf("ICON: reading new icon from %s\n", icon_path);
		unsigned char *icon_data = read_file(icon_path, &icon_size);
		if (icon_data == nullptr) {
			printf("ICON: no icon set in editor, using default icon\n");
			icon_data = (unsigned char *) icon_default;
			icon_size = icon_default_size;
			icon = 0;
		}

		// don't overwrite icon if size > dummy icon size
		if(icon_size > asset_hdr->icon.size) {
			printf("error: new icon size (%li) > %li\n", icon_size, asset_hdr->icon.size);
			if(icon) {
				free(icon_data);
			}
			free(asset_hdr);
			free(nro_hdr);
			return ERR_INVALID_DATA;
		}

		printf("ICON: writing new icon, %li bytes @ %li\n", icon_size, icon_offset);
		wrote = write_bytes(nro_path, icon_offset, icon_size, icon_data);
		if (wrote != icon_size) {
			printf("error: could not write icon data\n");
			if(icon) {
				free(icon_data);
			}
			free(asset_hdr);
			free(nro_hdr);
			return ERR_INVALID_DATA;
		}
		if(icon) {
			free(icon_data);
		}

		// update asset header
		printf("ASSET: writing new asset header, %li bytes @ %i\n", sizeof(AssetHeader), nro_hdr->size);
		asset_hdr->icon.size = icon_size;
		wrote = write_bytes(nro_path, nro_hdr->size, sizeof(AssetHeader), (const unsigned char *) asset_hdr);
		if (wrote != sizeof(AssetHeader)) {
			printf("error: could not write asset header\n");
			free(asset_hdr);
			free(nro_hdr);
			return ERR_INVALID_DATA;
		}

		// read nacp to memory
		if(asset_hdr->nacp.size > 0) {
			// asset_hdr->nacp.size should always be > 0 as we pack it with elf2nro @SCsub
			size_t nacp_offset = nro_hdr->size + asset_hdr->nacp.offset;
			printf("NACP: reading nacp, %li bytes @ %li\n", asset_hdr->nacp.size, nacp_offset);
			nacp = reinterpret_cast<NacpStruct *>(read_bytes(nro_path, nacp_offset, asset_hdr->nacp.size));
			if (nacp == nullptr) {
				printf("error: could not read nacp\n");
				free(asset_hdr);
				free(nro_hdr);
				return ERR_INVALID_DATA;
			}
			// nacp read success
			// update nacp title and author
			for (int i = 0; i < 12; i++) {
				strncpy(nacp->lang[i].name, title, sizeof(nacp->lang[i].name) - 1);
				strncpy(nacp->lang[i].author, author, sizeof(nacp->lang[i].author) - 1);
			}
			strncpy(nacp->version, version, sizeof(nacp->version) - 1);
			// write nacp back
			printf("NACP: writing new nacp, %li bytes @ %li\n", asset_hdr->nacp.size, nacp_offset);
			wrote = write_bytes(nro_path, nacp_offset, asset_hdr->nacp.size, (const unsigned char *) nacp);
			if (wrote != asset_hdr->nacp.size) {
				printf("error: could not write nacp\n");
				free(nacp);
				free(asset_hdr);
				free(nro_hdr);
				return ERR_INVALID_DATA;
			}
			free(nacp);
		}

		printf("update_nro: done\n");
		free(asset_hdr);
		free(nro_hdr);

		return OK;
	}

	EditorExportPlatformSwitch() {
		
		Ref<Image> img = memnew(Image(_switch_logo));
		logo.instance();
		logo->create_from_image(img);
	}
	
	~EditorExportPlatformSwitch() {
	}
};

///
/// SWITCH NRO UTILS
///
unsigned char *read_file(const char *fn, size_t *len_out) {

	FILE *fd = fopen(fn, "rb");
	if (fd == nullptr) {
		printf("read_file: error: fopen failed\n");
		return nullptr;
	}

	fseek(fd, 0, SEEK_END);
	size_t len = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	auto buf = (unsigned char *) malloc(len);
	if (buf == nullptr) {
		printf("read_file: error: malloc failed\n");
		fclose(fd);
		return nullptr;
	}

	size_t rc = fread(buf, 1, len, fd);
	if (rc != len) {
		fclose(fd);
		free(buf);
		printf("read_file: error: fread failed\n");
		return nullptr;
	}

	fclose(fd);

	*len_out = len;
	return buf;
}

unsigned char *read_bytes(const char *fn, size_t off, size_t len) {

	FILE *fd = fopen(fn, "rb");
	if (fd == nullptr) {
		printf("read_bytes: error: fopen failed\n");
		return nullptr;
	}

	fseek(fd, 0, SEEK_END);
	size_t size = ftell(fd);
	if (off + len > size) {
		printf("read_bytes: error: offset + len > size\n");
		fclose(fd);
		return nullptr;
	}

	auto buf = (unsigned char *) malloc(len);
	if (buf == nullptr) {
		printf("read_file: error: malloc failed\n");
		fclose(fd);
		return nullptr;
	}

	fseek(fd, off, SEEK_SET);
	size_t rc = fread(buf, 1, len, fd);
	if (rc != len) {
		fclose(fd);
		free(buf);
		printf("read_file: error: fread failed\n");
		return nullptr;
	}

	fclose(fd);

	return buf;
}

size_t write_bytes(const char *fn, size_t off, size_t len, const unsigned char *data) {

	FILE *fd = fopen(fn, "rb+");
	if (fd == nullptr) {
		printf("write_bytes: error: fopen failed\n");
		return 0;
	}

	fseek(fd, 0, SEEK_END);
	size_t size = ftell(fd);
	if (off + len > size) {
		printf("write_bytes: error: offset + len > size\n");
		fclose(fd);
		return 0;
	}

	fseek(fd, off, SEEK_SET);
	size_t rc = fwrite(data, 1, len, fd);
	if (rc != len) {
		fclose(fd);
		printf("write_bytes: error: fwrite failed, written = %li\n", rc);
		return 0;
	}

	fclose(fd);

	return rc;
}

void register_switch_exporter() {

	Ref<EditorExportPlatformSwitch> exporter = Ref<EditorExportPlatformSwitch>(memnew(EditorExportPlatformSwitch));
	EditorExport::get_singleton()->add_export_platform(exporter);
}
