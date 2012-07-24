#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "ftdi.h"
#include "io.h"

#define MODE_NORMAL       0
#define MODE_SHOW_VERSION 1
#define MODE_SHOW_USAGE   2
#define MODE_LISTPORT     3

#define FTDI_VENDORID     0x0403
#define FT2232_PRODUCTID  0x6010
#define NORMAL_PRODUCTID  0x6001

char serial_number[256];
long baud = 115200;
bool shouldloaddriver = false;
int interface = INTERFACE_ANY;

#define debug printf

void cleanup(void) {
	//for Mac OS X
	if (shouldloaddriver) {
		printf("\r\nReloading FTDIUSBSerialDriver\n");
		system(
				"sudo kextload /System/Library/Extensions/FTDIUSBSerialDriver.kext");
	}
}

void cleanup2(int i) {
	exit(1);
}

void init(void) {
	//for Mac OS X
	int ret = system(
			"kextstat|grep -c com.FTDI.driver.FTDIUSBSerialDriver > /dev/null");
	if (!ret) {
		shouldloaddriver = true;
		printf("Unloading FTDIUSBSerialDriver\n");
		ret
				= system(
						"sudo kextunload /System/Library/Extensions/FTDIUSBSerialDriver.kext");
		if (ret != 0) {
			fprintf(stderr, "Failed to unload driver. (%d)\n", ret);
			exit(1);
		}
	}
	atexit(cleanup);
	signal(SIGINT, cleanup2);
}

void version(void) {
	printf("FT2232 Direct Serial Console\n"
		"Version 0.1\n"
		"Okamura Yasunobu All Rights Reserved.\n"
		"License by GPL\n");
}

void usage(void) {
	printf("usage: ftditerm [-?] [serial number]\n"
		"\n"
		"-?  --help        Show Help (this)\n"
		"--version         Show Version\n"
		"--list            List up devices\n\n"
		"-A                Use A interface (for FT2232)\n"
		"-B                Use B interface (for FT2232)\n"
		"Ctrl-C  Ctrl-D    Quit\n");
}

int analyze_arg(int argc, char **argv) {
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			strncpy(serial_number, argv[i], sizeof(serial_number));
		} else {
			if (argv[i][1] != '-') {//short argumnet
				switch (argv[i][1]) {
				case '?':
					return MODE_SHOW_USAGE;
				case 'b':
					sscanf(argv[++i], "%ld", &baud);
					break;
				case 'l':
					return MODE_LISTPORT;
				case 'A':
					interface = INTERFACE_A;
					break;
				case 'B':
					interface = INTERFACE_B;
					break;
				default:
					fprintf(stderr,"Unknown option -%c\n",argv[i][1]);
					return MODE_SHOW_USAGE;
				}
			} else {//long argumnet
				if (strcmp(argv[i] + 2, "list") == 0) {
					return MODE_LISTPORT;
				} else if (strcmp(argv[i] + 2, "version") == 0) {
					return MODE_SHOW_VERSION;
				} else if (strcmp(argv[i] + 2, "help") == 0) {
					return MODE_SHOW_USAGE;
				}
			}
		}
	}
	return MODE_NORMAL;
}

void listup_port(void) {
	struct ftdi_context context;
	struct ftdi_device_list *list, *enu;
	int num;
	char manufactrue[256];
	char description[256];
	char serial[256];
	char found = 0;

	ftdi_init(&context);

	num = ftdi_usb_find_all(&context, &list, FTDI_VENDORID, FT2232_PRODUCTID);

	if (num < 0) {
		printf("Failed to search device.\n");
		goto finish;
	} else {
		if (num > 0) {
			found = 1;
			printf("FT2232 : %d device(s) found.\n", num);
			enu = list;
			do {
				ftdi_usb_get_strings(&context, enu->dev, manufactrue,
						sizeof(manufactrue), description, sizeof(description),
						serial, sizeof(serial));
				printf("\nManufactrue   : %s\n", manufactrue);
				printf("Description   : %s\n", description);
				printf("Serial Number : %s\n", serial);
			} while ((enu = enu->next) != 0);
		}
	}

	num = ftdi_usb_find_all(&context, &list, FTDI_VENDORID, NORMAL_PRODUCTID);

	if (num < 0) {
		printf("Failed to search device.\n");
	} else {
		if (num > 0) {
			found = 1;
			printf("Normal Device : %d device(s) found.\n", num);
			enu = list;
			do {
				ftdi_usb_get_strings(&context, enu->dev, manufactrue,
						sizeof(manufactrue), description, sizeof(description),
						serial, sizeof(serial));
				printf("\nManufactrue   : %s\n", manufactrue);
				printf("Description   : %s\n", description);
				printf("Serial Number : %s\n", serial);
			} while ((enu = enu->next) != 0);
		}
	}

	finish: if (!found) {
		printf("Not Found.\n");
	}

	ftdi_deinit(&context);
}

int main(int argc, char** argv) {
	struct ftdi_context context;
	int ftStatus;
	int mode = analyze_arg(argc, argv);

	switch (mode) {
	case MODE_SHOW_VERSION:
		version();
		exit(0);
	case MODE_SHOW_USAGE:
		usage();
		exit(0);
	case MODE_LISTPORT:
		init();
		listup_port();
		exit(0);
	}

	init();
	ftdi_init(&context);

	ftdi_set_interface(&context, interface);

	if (strlen(serial_number)) {
		debug("Use %s\n", serial_number);
		ftStatus = ftdi_usb_open_desc(&context, FTDI_VENDORID,
				FT2232_PRODUCTID, 0, serial_number);
	} else {
		ftStatus = ftdi_usb_open(&context, FTDI_VENDORID, FT2232_PRODUCTID);
	}

	if (ftStatus == 0) {
		debug("Connection Succeed\n");
	} else {
		fprintf(stderr, "Error on opening device.\n%s\n",
				ftdi_get_error_string(&context));
		exit(1);
	}

	ftStatus = ftdi_set_baudrate(&context, baud);

	if (ftStatus == 0) {
		printf("Baudrate %ld    ", baud);
	} else {
		fprintf(stderr, "Error on setting baudrate\n%s\n",
				ftdi_get_error_string(&context));
		exit(1);
	}

	ftStatus = ftdi_set_line_property(&context, BITS_8, STOP_BIT_1, NONE);
	if (ftStatus == 0) {
		printf("8 Bits   1 Stop bit   None Parity\n");
	} else {
		fprintf(stderr, "Error on setting data characteristics\n%s\n",
				ftdi_get_error_string(&context));
		exit(1);
	}

	char sendbuf[256];
	char readbuf[1024*4];
	memset(readbuf, 0, sizeof(readbuf));
	long sendlen;
	long readlen;

	//FT_SetTimeouts(ftHandle,100,0);

	debug("Enter RAW Mode.\n");
	console_init();

	bool run = true;
	while (run) {
		if (iskeydown()) {
			memset(sendbuf, 0, sizeof(sendbuf));
			int len = cget(sendbuf, sizeof(sendbuf) - 1);
			if (sendbuf[0] == 0x03) {
				goto quit;
			}
			sendlen = ftdi_write_data(&context, (unsigned char*) sendbuf, len);
			if (sendlen < 0) {
				fprintf(stderr, "Send Error\r\n%s\r\n", ftdi_get_error_string(
						&context));
				goto quit;
			}
			if (len != sendlen) {
				fprintf(stderr, "Send Error\r\n");
				goto quit;
			}
		}

		readlen = ftdi_read_data(&context, (unsigned char*) readbuf,
				sizeof(readbuf) - 1);
		if (readlen < 0) {
			fprintf(stderr, "Read Error\r\n%s\r\n", ftdi_get_error_string(
					&context));
			goto quit;
		}
		if (readlen) {
			cputn(readbuf, readlen);
		}
		usleep(1000);

	}

	quit:

	ftdi_usb_close(&context);
	ftdi_deinit(&context);

	return 0;
}
