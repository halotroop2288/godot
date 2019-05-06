#include "switch_wrapper.h"
#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "main/main.h"
#include "os_switch.h"

int main(int argc, char *argv[])
{
	socketInitializeDefault();
	nxlinkStdio();

	OS_Switch os;
	os.set_executable_path(argv[0]);

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

	socketExit();
	return os.get_exit_code();
}
