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
	msr_tracks_t tracks;
	int i;

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

	serial = msr_serial_open (device, &fd, MSR_BLOCKING, MSR_BAUD);

	if (serial == -1) {
		err(1, "Serial open of %s failed", device);
		exit(1);
	}

	/* Prepare the reader with a reset */
	msr_init (fd);

	/* Get the device model */
	msr_model (fd);
	/* Get the firmware version information */
	msr_fwrev (fd);

	/* Set the reader into Hi-Co mode */
	msr_set_hi_co (fd);

	/* Ram test */
	msr_ram_test (fd);

	/* Prepare the reader with a reset */
	msr_init (fd);

	do {
		bzero ((char *)&tracks, sizeof(tracks));

		for (i = 0; i < MSR_MAX_TRACKS; i++)
			tracks.msr_tracks[i].msr_tk_len = MSR_MAX_TRACK_LEN;

		printf("Ready to do a raw read. Please slide a card.\n");
		msr_raw_read (fd, &tracks);

	    /* If we didn't get any data, don't do this next part */
		msr_pretty_printer_bits(tracks);
	} while (1);

	/* We're finished */
	msr_serial_close (fd);
	exit(0);
}
