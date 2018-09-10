#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>

#include "main/main.h"
#include "os_switch.h"

int main(int argc, char *argv[]) {

	OS_Switch os;

	char *cwd = (char *)malloc(PATH_MAX);
	getcwd(cwd, PATH_MAX);

	Error err = Main::setup(argv[0], argc - 1, &argv[1]);
	if (err != OK) {
		free(cwd);
		return 255;
	}

	if (Main::start())
		os.run(); // it is actually the OS that decides how to run
	Main::cleanup();

	chdir(cwd);
	free(cwd);

	return os.get_exit_code();
}
