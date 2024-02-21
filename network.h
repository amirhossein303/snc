#include <arpa/inet.h>
#include <netdb.h>

#define ERRNO_INVALID_DOMAIN 4
#define ERRNO_COULD_NOT_RESOLVE_DOMAIN 5

#define MAX_ADDR_LIST 5
#define IPV4_LENGTH 15


int resolve_domain_name(char* domain_name, char addr[]) {
	struct hostent *host_entry;
	struct in_addr **addr_list;

	if ((host_entry = gethostbyname(domain_name)) == NULL) {
		return ERRNO_INVALID_DOMAIN;
	}
	addr_list = (struct in_addr **)host_entry->h_addr_list;
	if (addr_list[0] != NULL) {
		strcpy(addr, inet_ntoa(*addr_list[0]));
		return 0;
	}
	return ERRNO_COULD_NOT_RESOLVE_DOMAIN;
}