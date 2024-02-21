#include <sys/socket.h>


#define ERRNO_SOCKETOPT 6
#define ERRNO_SOCKET_BIND 7
#define ERRNO_SOCKET_LISTEN 8
#define ERRNO_SOCKET_ACCEPT 9

#define MAX_NUMBER_CLIENT 2


int create_socket_server(int port, int* server_fd) {
	struct sockaddr_in address;
	int new_socket;
	int opt = 1;

	socklen_t addrlen = sizeof(address);

	if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return ERRNO_SOCKET_ERROR;
	}
	if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		return ERRNO_SOCKETOPT;
	}
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (bind(*server_fd, (struct sockaddr*)&address, sizeof(address)) < 0 ){
		return ERRNO_SOCKET_BIND;
	}

	if (listen(*server_fd, MAX_NUMBER_CLIENT) < 0) {
		return ERRNO_SOCKET_LISTEN;
	}
	printf("Listening on 0.0.0.0 %d\n", port);

	if ((new_socket = accept(*server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
		return ERRNO_SOCKET_ACCEPT;
	}
}