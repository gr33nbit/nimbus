#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h> 

#include <netinet/in.h>

#include <ctime>


#include <iostream>

using namespace std;

typedef struct {

	int sock;
	struct sockaddr address;
	int addr_len;

} connection_t;

void * process(void * ptr){

	char * buffer;
	int len;
	connection_t * conn;
	long addr = 0;

	if(!ptr) pthread_exit(0);

	conn = (connection_t *)ptr;

	while(1){

		buffer = NULL;

		read(conn->sock, &len, sizeof(int));

		if(len > 0){

			addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
			buffer = (char *)malloc((len+1)*sizeof(char));
			buffer[len] = 0;
			
		}

		read(conn->sock, buffer, len);

		if(buffer != NULL){

			printf("%s", buffer);

		}

		if(buffer[0] == '/' && buffer[1] == 'q' && buffer[2] == 'u' && buffer[3] == 'i' && buffer[4] == 't'){

			cout << "quit time!" << endl;
			close(conn->sock);
			break;
		}

		free(buffer);
	}
}

int main(int argc, char ** argv){

	int sock = -1;
	struct sockaddr_in address;
	int port = 55000;
	connection_t * connection;
	pthread_t thread;
	int bindError = 0;

	//create socket
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock <= 0){

		fprintf(stderr, "%s: error: cannot create socket", argv[0]);
		return -3;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	for(bindError = 0; bindError <= 5; bindError++){

		if(bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in))){

			if(bindError == 5){

				fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0]);
				return -4;
			} 
			else{

				//increments the port by one
				port++;
				address.sin_port = htons(port);

			}
		
		}
		else{

			cout << "port number: " << port << endl;
		}

	}
	
	//listen to port
	if(listen(sock, 10) < 0){

		fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		return -5;
	}

	printf("%s: ready and listening\n", argv[0]);

	while(1){

		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, (unsigned int*)&connection->addr_len);
		if(connection->sock <= 0){

			free(connection);

		} 
		else{

			//start new thread
			pthread_create(&thread, 0, process, (void *)connection);
			pthread_detach(thread);
		}
	}
}