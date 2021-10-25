#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("Enter program, server address and port.\n");
		exit(EXIT_FAILURE);
	}

	const char *_command = argv[1];
	const char *_address = argv[2];
	const int _port = atoi(argv[3]);

	if (!_port || !_address) {
		fprintf(stderr, "Error while parsing port or address.\n");
		exit(EXIT_FAILURE);
	}

	addrinfo _addrinfo_request;
	addrinfo *_addrinfo_response;

	bzero(&_addrinfo_request, sizeof(_addrinfo_request));
	_addrinfo_request.ai_family = AF_INET;
	_addrinfo_request.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(_address, nullptr, &_addrinfo_request, &_addrinfo_response)) {
		fprintf(stderr, "Error in getaddrinfo() at line 30.\n");
		exit(EXIT_FAILURE);
	}

	sockaddr_in _client_address = *(sockaddr_in *)_addrinfo_response->ai_addr;
	_client_address.sin_port = htons(_port);
	freeaddrinfo(_addrinfo_response);

	int _server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		fprintf(stderr, "Error in socket() at line 41.\n");
		exit(EXIT_FAILURE);
	}

	if (connect(_server_socket, (sockaddr *)&_client_address, sizeof(_client_address)) < 0) {
		fprintf(stderr, "Error in connect() at line 46.\n");
		close(_server_socket);
		exit(EXIT_FAILURE);
	}

	pollfd _read_poll[2];
	_read_poll[0].fd = STDIN_FILENO;
	_read_poll[0].events = POLLIN;
	_read_poll[1].fd = _server_socket;
	_read_poll[1].events = POLLIN;

	printf("Client is prepared to connect.\n");

	printf("Send command: %s\n", _command);
	if (write(_server_socket, _command, sizeof(_command)) < 0) {
		fprintf(stderr, "Error in write(socket) at line 66.\n");
		close(_server_socket);
		exit(EXIT_FAILURE);
	}

	while (true) {
		char buffer[BUFFER_SIZE];

		if (poll(_read_poll, 2, -1) < 0) {
			fprintf(stderr, "Error in poll() at line 67.\n");
			break;
		}

		if (_read_poll[0].revents & POLLIN) {
			int size = read(STDIN_FILENO, buffer, sizeof(buffer));
			if (size < 0) {
				fprintf(stderr, "Error in read(stdin) at line 74.\n");
				break;
			}

			if (write(_server_socket, buffer, size) < 0) {
				fprintf(stderr, "Error in write(socket) at line 79.\n");
				break;
			}
		}

		if (_read_poll[1].revents & POLLIN) {
			int size = read(_server_socket, buffer, sizeof(buffer));
			if (size < 0) {
				fprintf(stderr, "Error in read(socket) at line 87.\n");
				break;
			}

			if (write(STDOUT_FILENO, buffer, size) < 0) {
				fprintf(stderr, "error in write(stdout) at line 92.\n");
				break;
			}
		}
	}

	close(_server_socket);
	return 0;
}
