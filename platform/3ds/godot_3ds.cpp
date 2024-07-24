#include "main/main.h"
#include "os_3ds.h"
#include "platform_config.h"
#include "servers/display_server.h"
#include <sys/resource.h>
#include <filesystem>

int __stacksize__ = STACKSIZE * 8;

int main(int argc, char *argv[]) {
	OS_3DS os;
	setlocale(LC_CTYPE, "");
	std::string without_filename = std::filesystem::path(argv[0]).remove_filename();
	chdir(without_filename.c_str());
	const char *path[6] = { "--main-pack", "romfs:/Game.pck", "--display-driver", "3ds", "--audio-driver", "Dummy" };
	Error err = Main::setup(argv[0], 6, (char**)path);
	if (err != OK) {
		printf("The error of setup was %i\n", err);
		if (err == ERR_HELP) { // Returned by --help and --version, so success.
			return 0;
		}
		return err;
	}
	if (Main::start() == EXIT_SUCCESS) {
		
		os.set_exit_code(EXIT_SUCCESS);
		os.run();
	}
	Main::cleanup();
	printf("Exit code : %d\n", os.get_exit_code());
	return os.get_exit_code();
}

extern "C" {
uint32_t htonl(uint32_t hostlong) {
	return __builtin_bswap32(hostlong);
}

uint16_t htons(uint16_t hostshort) {
	return __builtin_bswap16(hostshort);
}

uint32_t ntohl(uint32_t netlong) {
	return __builtin_bswap32(netlong);
}

uint16_t ntohs(uint16_t netshort) {
	return __builtin_bswap16(netshort);
}
}
