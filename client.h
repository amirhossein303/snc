#include <arpa/inet.h>
#include <sys/socket.h>


#define ERRNO_SOCKET_ERROR 1
#define ERRNO_CONNECTION_FAILED 2
#define ERRNO_INVALID_ADDR 3


int socket_client_connect(char* address, int port, int* client_fd) {
	struct sockaddr_in serv_addr;
	int status;

	if ((*client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return ERRNO_SOCKET_ERROR;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
		return ERRNO_INVALID_ADDR;
	}

	if ((status = connect(*client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		return ERRNO_CONNECTION_FAILED;
	}
}
