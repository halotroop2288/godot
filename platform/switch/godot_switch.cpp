#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "main/main.h"
#include "os_switch.h"

#include <switch.h>
#include <netinet/in.h>

#define ENABLE_NXLINK

#ifndef ENABLE_NXLINK
#define TRACE(fmt,...) ((void)0)
#else
#include <unistd.h>
#define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)

static int s_nxlinkSock = -1;

static void initNxLink()
{
	if (R_FAILED(socketInitializeDefault()))
		return;

	s_nxlinkSock = nxlinkStdio();
	if (s_nxlinkSock >= 0)
		TRACE("printf output now goes to nxlink server");
	else
		socketExit();
}

static void deinitNxLink()
{
	if (s_nxlinkSock >= 0)
	{
		close(s_nxlinkSock);
		socketExit();
		s_nxlinkSock = -1;
	}
}
#endif


int main(int argc, char *argv[])
{
	initNxLink();

	OS_Switch os;

	char *cwd = (char *)malloc(PATH_MAX);
	getcwd(cwd, PATH_MAX);

	Error err = Main::setup(argv[0], argc - 1, &argv[1]);
	if (err != OK) {
		free(cwd);
		return 255;
	}

	if (Main::start())
	{
		os.run(); // it is actually the OS that decides how to run
	}
	Main::cleanup();

	chdir(cwd);
	free(cwd);

	deinitNxLink();

	return os.get_exit_code();
}
