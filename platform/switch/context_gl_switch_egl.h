#pragma once
#include "drivers/gl_context/context_gl.h"
#include <EGL/egl.h>    // EGL library

class ContextGLSwitchEGL : public ContextGL
{
	bool gles3_context;
	bool vsync;

	EGLDisplay display;
	EGLContext context;
	EGLSurface surface;
public:

	virtual void release_current();
	virtual void make_current();

	virtual int get_window_width();
	virtual int get_window_height();
	virtual void swap_buffers();

	void set_use_vsync(bool use) { vsync = use; }
	bool is_using_vsync() const { return vsync; }

	virtual Error initialize();
	void reset();
	void cleanup();

	ContextGLSwitchEGL(bool gles3);
	virtual ~ContextGLSwitchEGL();
};