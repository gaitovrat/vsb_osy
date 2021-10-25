#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Enter port number.\n");
		exit(EXIT_FAILURE);
	}

	int _port = atoi(argv[1]);
	if (_port <= 0) {
		fprintf(stderr, "Error in atoi() at line 14\n");
		exit(EXIT_FAILURE);
	}

	int _server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket == -1) {
		fprintf(stderr, "Error in socket() at line 20\n");
		exit(EXIT_FAILURE);
	}

	int _socket_options = 1;
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &_socket_options,
			sizeof(_socket_options)) < 0) {
		fprintf(stderr, "Error in setsockopt() at line 34\n");
		close(_server_socket);
		exit(EXIT_FAILURE);
	}

	sockaddr_in _server_address = { .sin_family = AF_INET, .sin_port = htons(
			_port), .sin_addr = { INADDR_ANY } };

	if (bind(_server_socket, (const sockaddr*) &_server_address,
			sizeof(_server_address)) < 0) {
		fprintf(stderr, "Error in bind() at line 40\n");
		close(_server_socket);
		exit(EXIT_FAILURE);
	}

	if (listen(_server_socket, 1) < 0) {
		fprintf(stderr, "Error in listen() at line 46\n");
		close(_server_socket);
		exit(EXIT_FAILURE);
	}

	printf("Server is prepaired to run.\n");

	while (true) {
		int _client_socket = -1;
		pollfd _read_poll[2];
		_read_poll[0].fd = STDIN_FILENO;
		_read_poll[0].events = POLLIN;
		_read_poll[1].fd = _server_socket;
		_read_poll[1].events = POLLIN;

		printf("Waiting for new client.\n");

		while (true) {
			if (poll(_read_poll, 2, -1) < 0) {
				fprintf(stderr, "Error in poll() at line 65\n");
				close(_server_socket);
				exit(EXIT_FAILURE);
			}

			if (_read_poll[0].revents & POLLIN) {
				char buffer[BUFFER_SIZE];
				if (read(STDIN_FILENO, buffer, sizeof(buffer)) < 0) {
					fprintf(stderr, "Error in read(stdin) at line 75.\n");
					close(_server_socket);
				}
			}

			if (_read_poll[1].revents & POLLIN) {
				sockaddr_in _client_address;
				int _size = sizeof(_client_address);
				_client_socket = accept(_server_socket,
						(sockaddr*) &_client_address, (socklen_t*) &_size);
				if (_client_socket == -1) {
					fprintf(stderr, "Error in accept() at line 91.\n");
					close(_server_socket);
					exit(EXIT_FAILURE);
				}
				break;
			}
		}

		_read_poll[1].fd = _client_socket;

		printf("Start communication with client.\n");

		if (fork() == 0) {
			while (true) {
				char buffer[BUFFER_SIZE];
				if (poll(_read_poll, 2, -1) < 0) {
					fprintf(stderr, "Error in poll() at line 105.\n");
					close(_client_socket);
					close(_server_socket);
					exit(EXIT_FAILURE);
				}

				if (_read_poll[0].revents & POLLIN) {
					int _size = read(STDIN_FILENO, buffer, sizeof(buffer));
					if (_size < 0) {
						fprintf(stderr, "Error in read(stdin) at line 114.\n");
						close(_client_socket);
						close(_server_socket);
						exit(EXIT_FAILURE);
					}

					if (write(_client_socket, buffer, _size) < 0) {
						fprintf(stderr,
								"Error in write(socket) at line 121.\n");
						close(_client_socket);
						close(_server_socket);
						exit(EXIT_FAILURE);
					}
				}

				if (_read_poll[1].revents & POLLIN) {
					int _size = read(_client_socket, buffer, sizeof(buffer));
					if (_size < 0) {
						fprintf(stderr, "Error in read(socket) at line 131\n");
						close(_client_socket);
						close(_server_socket);
						exit(EXIT_FAILURE);
					}

					if (!strncmp(buffer, "close", sizeof("close"))) {
						printf("Close connection with client\n");
						close(_client_socket);
						break;
					}

					char *_command[BUFFER_SIZE];
					int i = 0;
					int n = 0;
					int _len = strlen(buffer);
					while (i < _len) { // split line to array of words
						while (i < _len && isspace(buffer[i]))
							buffer[i++] = '\0';
						if (i >= _len)
							break;

						_command[n++] = &buffer[i];

						while (i < _len && !isspace(buffer[i]))
							i++;
					}

					_command[n] = nullptr;
					if (!n) {
						if (write(STDOUT_FILENO, buffer, _size) < 0) {
							fprintf(stderr,
									"Error in write(stdout) at line 143.\n");
							close(_client_socket);
							close(_server_socket);
							exit(EXIT_FAILURE);
						}
					} else if (fork() == 0) {
						close(_server_socket);
						dup2(_client_socket, STDOUT_FILENO);
						close(_client_socket);

						execvp(_command[0], _command);

						fprintf(stderr, "Error in execvp() at line 169.\n");
						exit(EXIT_FAILURE);
					}
				}
			}
		}
	}

	return 0;
}
