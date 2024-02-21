#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snc.h"

#define PROGNAME "snc"
#define PROGDESC "Secure Netcat"
#define VERSION "1.0.0"

Flags flags = {
	.listen_flag=0,
	.port=0,
	.host=NULL
};


void help(void) {
	printf("Usage: %s [--version] [--help] [--port <PORT>] [--host <IP/Domain>] [--listen]\n", PROGNAME);
	printf("Options:\n");
	printf(" -h, --help\t\tPrint out this help text\n");
	printf(" -V, --version\t\tDisplay version number\n");
	printf(" -p, --port\t\tSpecify port to connect/listen\n");
	printf(" -H, --host\t\tSPecify host ip/domain\n");
	printf(" -l, --listen\t\tif this options given, only listen on given port of localhost\n");
}


void version(void) {
	printf("%s (%s) version: %s\n", PROGNAME, PROGDESC, VERSION);
}


int valid_port(char* port) {
	return (port != NULL &&
			strlen(port) > 0 &&
			isalldigit(port) &&
			((atoi(port)>0 && atoi(port) <= 65536)));
}


int valid_host(char* host) {
	return (host != NULL && 
			strlen(host) > 0);
}


int isalldigit(char* num_as_string) {
	char c;
	while ((c=*num_as_string++) != '\0') {
		if (c < '0' || c > '9') {
			return 0;
		}
	}
	return 1;
}


int is_ipv4(char* host) {
	char* token;
	int o = 0;
	char ahost[strlen(host)];
	sprintf(ahost, host, strlen(host));
	token = strtok(ahost, ".");
	while (token != NULL) {
		if (strlen(token) > 0 && isalldigit(token) && atoi(token) >= 0 && atoi(token) <= 255) {
			o++;
		} else {
			return 0;
		}
		token = strtok(NULL, ".");
	}
	return o == 4;
}


int main(int argc, char **argv) {
	int c;

	for (argc--, argv++; argc; argv++, argc--) {
		if ((strcmp(*argv, "-h") == 0) || strcmp(*argv, "--help") == 0) {
			help();
			exit(0);
		} else if ((strcmp(*argv, "-V") == 0) || (strcmp(*argv, "--version") == 0)) {
			version();
			exit(0);
		} else if ((strcmp(*argv, "--port") == 0) || (strcmp(*argv, "-p") == 0)) {
			if (!valid_port(*(argv+1))) {
				printf("%s: options '%s' has invalid value '%s'\n", PROGNAME, *argv, *(argv+1));
				printf("valid port: [0-65536], ports less than 1024 need superuser privilage\n");
				exit(1);
			}
			argv++; argc--;
			flags.port = atoi(*argv);
		} else if ((strcmp(*argv, "--host") == 0) || (strcmp(*argv, "-H") == 0)) {
			if (!valid_host(*(argv+1))) {
				printf("%s: options '%s' is invalid: '%s', please enter IPv4/Domain\n", PROGNAME, *argv, *(argv+1));
				exit(1);
			}
			argc--, argv++;
			flags.host = *argv;
		} else if ((strcmp(*argv, "--listen") == 0) || (strcmp(*argv, "-l") == 0)) {
			flags.listen_flag = 1;
		} else {
			printf("%s: options '%s': is unknown\n", PROGNAME, *argv);
			printf("%s: try '%s --help' or '%s --h' for more information\n", PROGNAME, PROGNAME, PROGNAME);
			exit(2);
		}
	}
	if (!flags.listen_flag && flags.host == NULL) {
		printf("%s: options --host is required while --listen not specified\n", PROGNAME);
		exit(2);
	}
	if (flags.port == 0) {
		printf("%s: options --port is required\n", PROGNAME);
		exit(2);
	}
	printf("listen flag: %s\nhost: %s\nport: %d\n", (flags.listen_flag ? "yes" : "no"), flags.host, flags.port);
}