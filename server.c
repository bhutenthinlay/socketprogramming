#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(int argc, char *argv[]){
	struct sockaddr_un name;
	int ret;
	int connection_socket;
	int data_socket;
	int data;
	char buffer[BUFFER_SIZE];
	int result;
	/* In case the program exited inadvertently  on the last run, remove the socket*/
	unlink(SOCKET_NAME);
	connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (connection_socket == -1){
		perror("socket");
		return EXIT_FAILURE;
	}
	printf("Master socket created\n");
	//initialize
	memset(&name, 0, sizeof(struct sockaddr_un));
	//specify the socket credentials
	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path)-1);

	//bind the socket name
	ret = bind(connection_socket, (const struct sockaddr *)&name, sizeof(struct sockaddr_un));
	if(ret == -1){
		perror("Bind");
		return EXIT_FAILURE;
	}
	printf("Bind successful\n");
	//need to listen
	ret = listen(connection_socket, 5);
	if(ret == -1){
		perror("Listen");
		return EXIT_FAILURE;
	}
	printf("Listen successful");
	while(1){
		printf("waiting for accept() call\n");
		data_socket = accept(connection_socket, NULL, NULL);
		if(data_socket == -1){
			perror("accept");
			return EXIT_FAILURE;
		}
		printf("connection accepted from a client\n");
		while(1){
			memset(buffer, 0, BUFFER_SIZE);
			printf("waiting for data from the client\n");
			ret = read(data_socket, buffer, BUFFER_SIZE);
			if(ret == -1){
				perror("read");
				return EXIT_FAILURE;
			}
			memcpy(&data, buffer, sizeof(int));
			if(data == 0)
				break;
			result += data;
		}
		memset(buffer, 0, BUFFER_SIZE);
		sprintf(buffer, "Result = %d", result);
		printf("sendinf final result back to client\n");
		ret = write(data_socket, buffer, BUFFER_SIZE);
		if(ret == -1){
			perror("write");
			return EXIT_FAILURE;
		}
		close(data_socket);
	}
	close(connection_socket);
	return EXIT_SUCCESS;
}