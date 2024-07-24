/**************************************************************************/
/*  dir_access_unix.cpp                                                   */
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
#include "dir_access_3ds.h"

#include "core/os/memory.h"
#include "core/os/os.h"
#include "core/string/print_string.h"
#include "core/templates/list.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <string>

Error DirAccess3DS::list_dir_begin() {
	list_dir_end(); //close any previous dir opening!

	//char real_current_dir_name[2048]; //is this enough?!
	//getcwd(real_current_dir_name,2048);
	//chdir(current_path.utf8().get_data());
	dir_stream = opendir(current_dir.utf8().get_data());
	//chdir(real_current_dir_name);
	if (!dir_stream) {
		return ERR_CANT_OPEN; //error!
	}

	return OK;
}

bool DirAccess3DS::file_exists(String p_file) {
	GLOBAL_LOCK_FUNCTION

	if (p_file.is_relative_path()) {
		p_file = current_dir.path_join(p_file);
	}

	p_file = fix_path(p_file);

	struct stat flags = {};
	bool success = (stat(p_file.utf8().get_data(), &flags) == 0);

	if (success && S_ISDIR(flags.st_mode)) {
		success = false;
	}

	return success;
}

bool DirAccess3DS::dir_exists(String p_dir) {
	GLOBAL_LOCK_FUNCTION

	if (p_dir.is_relative_path()) {
		p_dir = get_current_dir().path_join(p_dir);
	}

	p_dir = fix_path(p_dir);

	struct stat flags = {};
	bool success = (stat(p_dir.utf8().get_data(), &flags) == 0);

	return (success && S_ISDIR(flags.st_mode));
}

bool DirAccess3DS::is_readable(String p_dir) {
	GLOBAL_LOCK_FUNCTION

	if (p_dir.is_relative_path()) {
		p_dir = get_current_dir().path_join(p_dir);
	}

	p_dir = fix_path(p_dir);
	return (access(p_dir.utf8().get_data(), R_OK) == 0);
}

bool DirAccess3DS::is_writable(String p_dir) {
	GLOBAL_LOCK_FUNCTION

	if (p_dir.is_relative_path()) {
		p_dir = get_current_dir().path_join(p_dir);
	}

	p_dir = fix_path(p_dir);
	return (access(p_dir.utf8().get_data(), W_OK) == 0);
}

uint64_t DirAccess3DS::get_modified_time(String p_file) {
	if (p_file.is_relative_path()) {
		p_file = current_dir.path_join(p_file);
	}

	p_file = fix_path(p_file);

	struct stat flags = {};
	bool success = (stat(p_file.utf8().get_data(), &flags) == 0);

	if (success) {
		return flags.st_mtime;
	} else {
		ERR_FAIL_V(0);
	}
	return 0;
}

String DirAccess3DS::get_next() {
	if (!dir_stream) {
		return "";
	}

	dirent *entry = readdir(dir_stream);

	if (entry == nullptr) {
		list_dir_end();
		return "";
	}

	String fname = fix_unicode_name(entry->d_name);

	// Look at d_type to determine if the entry is a directory, unless
	// its type is unknown (the file system does not support it) or if
	// the type is a link, in that case we want to resolve the link to
	// known if it points to a directory. stat() will resolve the link
	// for us.
	if (entry->d_type == DT_UNKNOWN || entry->d_type == DT_LNK) {
		String f = current_dir.path_join(fname);

		struct stat flags = {};
		if (stat(f.utf8().get_data(), &flags) == 0) {
			_cisdir = S_ISDIR(flags.st_mode);
		} else {
			_cisdir = false;
		}
	} else {
		_cisdir = (entry->d_type == DT_DIR);
	}

	_cishidden = is_hidden(fname);

	return fname;
}

bool DirAccess3DS::current_is_dir() const {
	return _cisdir;
}

bool DirAccess3DS::current_is_hidden() const {
	return _cishidden;
}

void DirAccess3DS::list_dir_end() {
	if (dir_stream) {
		closedir(dir_stream);
	}
	dir_stream = nullptr;
	_cisdir = false;
}

static void _get_drives(List<String> *list) {
	
	char sdmc_path[256];
	FSUSER_GetSdmcCtrRootPath((unsigned char*)sdmc_path,256);
	printf("got sdmc root path %s\n", sdmc_path);

	list->push_back(sdmc_path);

	list->sort();
}

int DirAccess3DS::get_drive_count() {
	List<String> list;
	_get_drives(&list);

	return list.size();
}

String DirAccess3DS::get_drive(int p_drive) {
		List<String> list;
	_get_drives(&list);

	ERR_FAIL_INDEX_V(p_drive, list.size(), "");

	return list.get(p_drive);
}

int DirAccess3DS::get_current_drive() {
	int drive = 0;
	int max_length = -1;
	const String path = get_current_dir().to_lower();
	for (int i = 0; i < get_drive_count(); i++) {
		const String d = get_drive(i).to_lower();
		if (max_length < d.length() && path.begins_with(d)) {
			max_length = d.length();
			drive = i;
		}
	}
	return drive;
}

bool DirAccess3DS::drives_are_shortcuts() {
	return true;
}

Error DirAccess3DS::make_dir(String p_dir) {
	GLOBAL_LOCK_FUNCTION

	if (p_dir.is_relative_path()) {
		p_dir = get_current_dir().path_join(p_dir);
	}

	p_dir = fix_path(p_dir);

	bool success = (mkdir(p_dir.utf8().get_data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
	int err = errno;

	if (success) {
		return OK;
	}

	if (err == EEXIST) {
		return ERR_ALREADY_EXISTS;
	}

	return ERR_CANT_CREATE;
}

Error DirAccess3DS::change_dir(String p_dir) {
	GLOBAL_LOCK_FUNCTION

	p_dir = fix_path(p_dir);

	// prev_dir is the directory we are changing out of
	String prev_dir;
	char real_current_dir_name[2048];
	ERR_FAIL_COND_V(getcwd(real_current_dir_name, 2048) == nullptr, ERR_BUG);
	if (prev_dir.parse_utf8(real_current_dir_name) != OK) {
		prev_dir = real_current_dir_name; //no utf8, maybe latin?
	}

	// try_dir is the directory we are trying to change into
	String try_dir = "";
	if (p_dir.is_relative_path()) {
		String next_dir = current_dir.path_join(p_dir);
		next_dir = next_dir.simplify_path();
		try_dir = next_dir;
	} else {
		try_dir = p_dir;
	}

	bool worked = (chdir(try_dir.utf8().get_data()) == 0); // we can only give this utf8
	if (!worked) {
		return ERR_INVALID_PARAMETER;
	}

	String base = _get_root_path();
	if (!base.is_empty() && !try_dir.begins_with(base)) {
		ERR_FAIL_COND_V(getcwd(real_current_dir_name, 2048) == nullptr, ERR_BUG);
		String new_dir;
		new_dir.parse_utf8(real_current_dir_name);

		if (!new_dir.begins_with(base)) {
			try_dir = current_dir; //revert
		}
	}

	// the directory exists, so set current_dir to try_dir
	current_dir = try_dir;
	ERR_FAIL_COND_V(chdir(prev_dir.utf8().get_data()) != 0, ERR_BUG);
	return OK;
}

String DirAccess3DS::get_current_dir(bool p_include_drive) const {
	String base = _get_root_path();
	if (!base.is_empty()) {
		String bd = current_dir.replace_first(base, "");
		if (bd.begins_with("/")) {
			return _get_root_string() + bd.substr(1, bd.length());
		} else {
			return _get_root_string() + bd;
		}
	}
	return current_dir;
}

Error DirAccess3DS::rename(String p_path, String p_new_path) {
	if (p_path.is_relative_path()) {
		p_path = get_current_dir().path_join(p_path);
	}

	p_path = fix_path(p_path);

	if (p_new_path.is_relative_path()) {
		p_new_path = get_current_dir().path_join(p_new_path);
	}

	p_new_path = fix_path(p_new_path);

	return ::rename(p_path.utf8().get_data(), p_new_path.utf8().get_data()) == 0 ? OK : FAILED;
}

Error DirAccess3DS::remove(String p_path) {
	if (p_path.is_relative_path()) {
		p_path = get_current_dir().path_join(p_path);
	}

	p_path = fix_path(p_path);

	struct stat flags = {};
	if ((stat(p_path.utf8().get_data(), &flags) != 0)) {
		return FAILED;
	}

	if (S_ISDIR(flags.st_mode)) {
		return ::rmdir(p_path.utf8().get_data()) == 0 ? OK : FAILED;
	} else {
		return ::unlink(p_path.utf8().get_data()) == 0 ? OK : FAILED;
	}
}

bool DirAccess3DS::is_link(String p_file) {
	return false;
}

String DirAccess3DS::read_link(String p_file) {
	return "";
}

Error DirAccess3DS::create_link(String p_source, String p_target) {
	return Error::ERR_UNAVAILABLE;
}

uint64_t DirAccess3DS::get_space_left() {
	struct statvfs vfs;
	if (statvfs(current_dir.utf8().get_data(), &vfs) != 0) {
		return 0;
	}

	return (uint64_t)vfs.f_bavail * (uint64_t)vfs.f_frsize;
}

String DirAccess3DS::get_filesystem_type() const {
	return ""; //TODO this should be implemented
}

bool DirAccess3DS::is_hidden(const String &p_name) {
	return p_name != "." && p_name != ".." && p_name.begins_with(".");
}

DirAccess3DS::DirAccess3DS() {
	dir_stream = nullptr;
	_cisdir = false;

	/* determine drive count */

	// set current directory to an absolute path of the current directory
	char real_current_dir_name[2048];
	ERR_FAIL_COND(getcwd(real_current_dir_name, 2048) == nullptr);
	if (current_dir.parse_utf8(real_current_dir_name) != OK) {
		current_dir = real_current_dir_name;
	}

	change_dir(current_dir);
}

DirAccess3DS::~DirAccess3DS() {
	list_dir_end();
}