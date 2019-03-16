#include "switch_wrapper.h"
#include "context_gl_switch_egl.h"
#include <stdio.h>

#define ENABLE_NXLINK
#ifndef ENABLE_NXLINK
#define TRACE(fmt,...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)
#endif

ContextGLSwitchEGL::ContextGLSwitchEGL(bool gles3_context)
{
	this->gles3_context = gles3_context;
}

ContextGLSwitchEGL::~ContextGLSwitchEGL()
{
    TRACE("Deleting EGL!\n");
	cleanup();
}

Error ContextGLSwitchEGL::initialize()
{
    TRACE("Starting EGL init!");
    // Connect to the EGL default display
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!display)
    {
        TRACE("Could not connect to display! error: %d", eglGetError());
        goto _fail0;
    }

    // Initialize the EGL display connection
    eglInitialize(display, NULL, NULL);

    // Get an appropriate EGL framebuffer configuration
    EGLConfig config;
    EGLint numConfigs;
    static const EGLint attributeList[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE,   8,
        EGL_DEPTH_SIZE,   24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    eglChooseConfig(display, attributeList, &config, 1, &numConfigs);
    if (numConfigs == 0)
    {
        TRACE("No config found! error: %d", eglGetError());
        goto _fail1;
    }

    // Create an EGL window surface
    surface = eglCreateWindowSurface(display, config, nwindowGetDefault(), NULL);
    if (!surface)
    {
        TRACE("Surface creation failed! error: %d", eglGetError());
        goto _fail1;
    }

    static const EGLint contextAttributeList[] =
    {
        EGL_CONTEXT_CLIENT_VERSION, 3, // request OpenGL ES 3.x
        EGL_NONE
    };

    // Create an EGL rendering context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributeList);
    if (!context)
    {
        TRACE("Context creation failed! error: %d", eglGetError());
        goto _fail2;
    }

    // Connect the context to the surface
    eglMakeCurrent(display, surface, surface, context);
    TRACE("egl shit done!!\n");
    return OK;

_fail2:
    eglDestroySurface(display, surface);
    surface = NULL;
_fail1:
    eglTerminate(display);
    display = NULL;
_fail0:
    return ERR_UNCONFIGURED;
}

void ContextGLSwitchEGL::cleanup()
{
    TRACE("Cleaning up!!!!!!");
    if (display)
    {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context)
        {
            eglDestroyContext(display, context);
            context = NULL;
        }
        if (surface)
        {
            eglDestroySurface(display, surface);
            surface = NULL;
        }
        eglTerminate(display);
        display = NULL;
    }
    TRACE("done cleaning up!!!!!!");
}

void ContextGLSwitchEGL::reset()
{
	cleanup();
	initialize();
}

void ContextGLSwitchEGL::release_current()
{
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void ContextGLSwitchEGL::make_current()
{
	eglMakeCurrent(display, surface, surface, context);
}

int ContextGLSwitchEGL::get_window_width()
{
	return 1280;
}

int ContextGLSwitchEGL::get_window_height()
{
	return 720; // todo: stub
}

void ContextGLSwitchEGL::swap_buffers()
{
    TRACE("Swap!");
	eglSwapBuffers(display, surface);
}
