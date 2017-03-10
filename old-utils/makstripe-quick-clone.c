#include <sys/types.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <strings.h>
#include <termios.h>
#include <err.h>
#include <string.h>

#include "libmsr.h"

int main(int argc, char * argv[])
{
	int fd = -1;
	int serial;
	int ret;

	/* Default device selection per platform */
#ifdef __linux__
	char *device = "/dev/ttyUSB0";
#else
	char *device = "/dev/cuaU0";
#endif

	if (argv[1] != NULL)
		device = argv[1];
	else
		printf ("no device specified, defaulting to %s\n", device);

	serial = msr_serial_open (device, &fd, MAK_BLOCKING, MAK_BAUD);

	if (serial == -1) {
		err(1, "Serial open of %s failed", device);
		exit(1);
	}
	printf("Ready to populate MAKStripe buffer...\n");
	ret = mak_successful_read(fd, MAKSTRIPE_TK_ALL);
	if (ret != 0) {
		printf("Unable to populate MAKStripe buffer!\n");
		exit(ret);
	}
	printf("Ready to clone buffer onto blank card...\n");
	ret = mak_successful_clone(fd);
	if (ret != 0) {
		printf("Unable to clone buffer onto blank card!\n");
		exit(ret);
	}
	msr_serial_close (fd);
	exit(ret);
}
