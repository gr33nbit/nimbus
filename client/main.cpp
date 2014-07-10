#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>


using namespace std;

int main(int argc, char **argv){
	
	int port;
	int sock = -1;
	struct sockaddr_in address;
	struct hostent * host;
	int len;
	char message[1024];
	
	if(argc != 2){

		printf("Usage: %s port\n", argv[0]);
		return -1;
	}

	port = atoi(argv[1]);


	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(!sock){

		fprintf(stderr, "%s error: cannot create socket\n", argv[0]);
		return -3;
	}


	//connect to server
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	host = gethostbyname("127.0.0.1");
	if(!host){

		fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
		return -4;
	}


	memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);

	if( connect(sock, (struct sockaddr *) &address, sizeof(address))){

		fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
		return -5;
	}

	for(int i = 0; 1; i++){

		fgets(message, 1023, stdin);
		
		len = strlen(message);

		if(message[0] == '/' && message[1] == 'q' && message[2] == 'u' && message[3] == 'i' && message[4] == 't'){

			write(sock, &len, sizeof(int));
			write(sock, message, len);
			break;
		}

		write(sock, &len, sizeof(int));
		write(sock, message, len);

	}
	

	close(sock);
	return 0;
}