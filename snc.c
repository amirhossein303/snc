#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "snc.h"
#include "client.h"
#include "network.h"
#include "server.h"

#define PROGNAME "snc"
#define PROGDESC "Secure Netcat"
#define VERSION "1.0.0"

Flags flags = {
	.listen_flag=0,
	.port=0,
	.host=NULL,
	.is_af_inet=0,
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


int is_ipv4(char* buf) {
	char* token;
	int o = 0;
	char ahost[strlen(buf)];
	sprintf(ahost, buf, strlen(buf));
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


int is_domain_name(char* buf) {
	char c;
	int i = 0;
	int dots = 0;
	if (strcmp(buf, "localhost") == 0) {
		return 1;
	}
	while ((c = *(buf++)) != '\0') {
		if ((i != 0 && c == '.') || isdigit(c) || isalpha(c) || c == '-') {
			if (c == '.') {
				dots++;
			}
			i++;
			continue;
		}
		return 0;
	}
	return 1 && dots>0;
}


int main(int argc, char **argv) {
	int c;
	if (argc == 1) {
		help();
		exit(2);
	}
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
		printf("%s: missing host, try --host, -H\n", PROGNAME);
		exit(2);
	}
	if (flags.port == 0) {
		printf("%s: missing port number, try --port, -p\n", PROGNAME);
		exit(2);
	}

	flags.is_af_inet = flags.host != NULL ? is_ipv4(flags.host) : 0;
	if (!flags.listen_flag && !flags.is_af_inet && !is_domain_name(flags.host)) {
		printf("%s: options --host is not a valid ip/domain, please enter valid ip/domain-name\n", PROGNAME);
		exit(1);
	}

	if (!flags.listen_flag) {
		// Connect to specific host:port
		char addr[IPV4_LENGTH];
		strcpy(addr, flags.is_af_inet?flags.host:"");

		if (!flags.is_af_inet &&
			is_domain_name(flags.host) &&
			resolve_domain_name(flags.host, addr) != 0) {
			printf("%s: invalid host given, or could not resolve hostname\n", PROGNAME);
			exit(1);
		}

		int client_fd;
		switch (socket_client_connect(addr, flags.port, &client_fd)) {
			case 0:
				perror("connection established sucessfully");
				break;
			case ERRNO_SOCKET_ERROR:
				perror("error in creating socket stream");
				break;
			case ERRNO_CONNECTION_FAILED:
				perror("could not connect");
				break;
			case ERRNO_INVALID_ADDR:
				perror("address is invalid");
				break;
			default:
				perror("something went wrong");
		}
	} else {
		int server_fd;
		switch (create_socket_server(flags.port, &server_fd)) {
			case 0:
				break;
			case ERRNO_SOCKET_ERROR:
				perror("error in creating socket stream");
				break;
			case ERRNO_SOCKETOPT:
				perror("error in set socket options");
				break;
			case ERRNO_SOCKET_BIND:
				perror("could not bind to localhost");
				break;
			case ERRNO_SOCKET_LISTEN:
				perror("could not listening");
				break;
			case ERRNO_SOCKET_ACCEPT:
				perror("could not accept");
				break;
			default:
				perror("something went wrong");
		}
	}
}