#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <sys/wait.h>

/* Pthreads */
#include <pthread.h>
#include <assert.h>
#include <stdlib.h>

void messageToSub(){
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1) {
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(6060);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect failed. Error");
	}
	
	puts("Connected\n");
	
	//keep communicating with server
	strcpy(message, "8080");
	if( send(sock , message , strlen(message) , 0) < 0) {
		puts("Send failed");
	} else {
        puts("Send ok");
    }
	close(sock);	
}

void acceptingPetition(int * client_sock){
	char client_message[2000];
	pid_t pid2;

	memset (client_message, 0, 2000);
	//Receive a message from client
	if (recv(*client_sock , client_message , 2000 , 0) > 0) {
		printf("received message in agent: %s\n", client_message);

		char * token = strtok(client_message, " ");
		token = strtok(NULL, " ");
		printf("%s token\n", token);

		//Container creation
		if(client_message[0] == '1'){
			pid2 = fork ();
			if (pid2 < 0) { /* error occurred */
				fprintf(stderr, "Fork Failed\n");
			}
			else if (pid2 == 0) { /* child process */
				execlp("docker", "docker", "run", "-di", "--name", token, "ubuntu:latest", "/bin/bash", NULL);
			}
			else { /* parent process */
				/* parent will wait for the child to complete */
				wait(NULL);
				sleep(10);
				strcpy(client_message, token);
				strcat(client_message, " 8080");
				send(*client_sock , client_message , strlen(client_message), 0);
			}
		}else if(client_message[0] == '2'){
			//Container stop

			pid2 = fork ();
			if (pid2 < 0) { /* error occurred */
				fprintf(stderr, "Fork Failed\n");
			}
			else if (pid2 == 0) { /* child process */
				execlp("docker", "docker", "stop", token, NULL);
			}
			else { /* parent process */
				/* parent will wait for the child to complete */
				wait(NULL);
				sleep(20);
				strcpy(client_message, token);
				strcat(client_message, " stoped");
				send(*client_sock , client_message , strlen(client_message), 0);
			}
		}else if(client_message[0] == '3'){
			//Container remove

			pid2 = fork ();
			if (pid2 < 0) { /* error occurred */
				fprintf(stderr, "Fork Failed\n");
			}
			else if (pid2 == 0) { /* child process */
				execlp("docker", "docker", "rm", token, NULL);
			}
			else { /* parent process */
				/* parent will wait for the child to complete */
				wait(NULL);
				sleep(20);
				strcpy(client_message, token);
				strcat(client_message, " removed");
				send(*client_sock , client_message , strlen(client_message), 0);
			}
		}
	}
}

int main(int argc , char *argv[]) {
	int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;  // https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/in.h
	char client_message[2000];
	pid_t pid, pid2;
	
	pid = fork ();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    else if (pid == 0) { /* child process  ----- Messege to subcribe host*/
        messageToSub();
    }
    else { /* parent process */
		// Create socket
		// AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol) 
		// SOCK_STREAM: TCP(reliable, connection oriented)
		// SOCK_DGRAM: UDP(unreliable, connectionless)
		// Protocol value for Internet Protocol(IP), which is 0
		socket_desc = socket(AF_INET , SOCK_STREAM , 0);
		if (socket_desc == -1) {
			printf("Could not create socket");
		}
		puts("Socket created");
		
		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(8080);
		
		//Bind the socket to the address and port number specified
		if( bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0) {
			//print the error message
			perror("bind failed. Error");
			return 1;
		}
		puts("bind done");
		
		// Listen
		// It puts the server socket in a passive mode, where it waits for the client 
		// to approach the server to make a connection. The backlog, defines the maximum 
		// length to which the queue of pending connections for sockfd may grow. If a connection 
		// request arrives when the queue is full, the client may receive an error with an 
		// indication of ECONNREFUSED.
		int flag = 0;
		pthread_t p;
		while(flag == 0){
			listen(socket_desc , 3);

			//Accept and incoming connection
			puts("Waiting for incoming connections to 8080 ... ");
			c = sizeof(struct sockaddr_in);

			//accept connection from an incoming client
			client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			if (client_sock < 0) {
				perror("accept failed");
				return 1;
			}
			puts("Connection accepted");

			// ENVIO POR HILO
			pthread_create(&p, NULL, (void *)acceptingPetition, &client_sock);
		}
    }

	return 0;
}