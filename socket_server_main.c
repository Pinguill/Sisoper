#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>

/*| Shared memory */
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

/* Pthreads */
#include <pthread.h>
#include <assert.h>

char map[100][100];
int contContainer = 0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int agentConnect(int portSelected, char * nameContainer){
	int sock;
	struct sockaddr_in server;
	char server_reply[2000];
	
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1) {
		printf("Could not create socket agent\n");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(portSelected);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
		perror("connect agent failed. Error ");
		return 1;
	} 
	
	puts("Connected to agent\n");

	if( send(sock , nameContainer , strlen(nameContainer) , 0) < 0) {
		puts("Send failed");
		return 1;
	} else {
        puts("send ok agent conect");
    }

	if( recv(sock , server_reply , 2000 , 0) < 0) {
		puts("recv failed");
	} else {
        puts("recv ok");
    }

	puts("Server reply :");
	puts(server_reply);
	
	if(nameContainer[0] == '1'){
		printf("Containers has increased\n");
		pthread_mutex_lock( &mutex1 );
		strcpy(map[contContainer], server_reply);
		contContainer += 1;
		pthread_mutex_unlock( &mutex1 );
	}else if(nameContainer[0] == '3'){
		char nameCopy[100];
		char mapCopy[100];
		char vacio[100];

		for(int i = 0; i < contContainer; i++){
			strcpy(mapCopy, map[i]);
			strcpy(nameCopy, nameContainer);
			char * token = strtok(nameCopy, " ");
			token = strtok(NULL, " ");
			char * token2 = strtok(mapCopy, " ");

			if(strcmp(token, token2) == 0){
				strcpy(map[i], vacio);
				printf("Containers has decreased\n");
			}
		}
	}
	

}

// socket_server_host1 port = 8080
// socket_server_host2 port = 9090
void randomPicker(char * name){
	int Rnumber;
	srand(time(NULL));
	Rnumber = rand() % 2; 
	if(Rnumber == 0){
		agentConnect(8080 , name);
	}else if(Rnumber == 1){
		agentConnect(9090, name);
	}
}

void searchHost(char * name){
	char containerName[100];
	char mapCopy[100];
	strcpy(containerName, name);
	char * token = strtok(name, " ");
	token = strtok(NULL, " ");
	for(int i = 0; i < contContainer; i++){
		strcpy(mapCopy, map[i]);
		char * token2 = strtok(mapCopy, " ");
		if(strcmp(token, token2) == 0){
			token2 = strtok(NULL, " ");
			int destinationPort = atoi(token2);
			printf("port destination %d, name %s\n", destinationPort, containerName);
			agentConnect(destinationPort, containerName);
		}
	}
}

void acceptingConectionSub(int * client_sock){
	int c, read_size, cont = 0;
	struct sockaddr_in server, client;
	char client_message[2000];

	memset(client_message, 0, 2000);
	
	if(recv(*client_sock , client_message , 2000 , 0) > 0) {
		const int SIZE = 4096;
		/* name of the shared memory object */
		const char *name = "OS";
		/* strings written to shared memory */
		const char *message = client_message;
		/* shared memory file descriptor */
		int fd;
		/* pointer to shared memory obect */
		char *ptr;

		fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    	if (fd == -1) {
			perror("open");
		}

		ftruncate (fd, SIZE);

		ptr = (char *) mmap (0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

		sprintf(ptr, "%s", message);
		ptr += strlen(message);
	}
}

void acceptingConectionAdmin(int * client_sock){
	char client_message[2000];
	memset(client_message, 0, 2000);
	//Receive a message from client
	if(recv(* client_sock , client_message , 2000 , 0) > 0) {
		pthread_t p1;
		printf("received message: %s\n", client_message);
		//Send the message back to client
		send(*client_sock , client_message , strlen(client_message), 0);
		printf("mensaje: %s\n", client_message);
		/* OPC 1 = CREAR */
		/* OPC 2 = STOP */
		/* OPC 3 = REMOVE */
		/* OPC 4 = LIST */
		if((client_message[0] == '1')){
			pthread_create(&p1, NULL, (void *)randomPicker, &client_message);
		}else if(client_message[0] == '2'){
			pthread_create(&p1, NULL, (void *)searchHost, &client_message);
		}else if(client_message[0] == '3'){ 
			pthread_create(&p1, NULL, (void *)searchHost, &client_message);
		}else if(client_message[0] == '4'){
			// List
		}
	}
}

void childProcess(){
	int socket_desc, client_sock, c, read_size, cont = 0;
	struct sockaddr_in server, client;  // https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/in.h
	char client_message[2000];
	
	// Create socket
	// AF_INET (IPv4 protocol) , AF_INET6 (IPv6 protocol) 
	// SOCK_STREAM: TCP(reliable, connection oriented)
	// SOCK_DGRAM: UDP(unreliable, connectionless)
	// Protocol value for Internet Protocol(IP), which is 0
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1) {
		printf("Could not create socket");
	}
	puts("Socket Subcribe host");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(6060);
	
	//Bind the socket to the address and port number specified
	if( bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0) {
		//print the error message
		perror("bind failed. Error");
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
		//accept connection from an incoming client
		listen(socket_desc , 3);
		//Accept and incoming connection
		puts("Waiting for incoming connections to 6060 Subcribe host...");
		c = sizeof(struct sockaddr_in);

		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if(client_sock < 0) {
			perror("accept failed");
		}
		cont += 1;
		puts("Connection accepted");

		pthread_create(&p, NULL, (void *)acceptingConectionSub, &socket_desc);
		if(cont == 2){
			flag = 1;
			printf("%d\n", flag);
		}
	}
}

int main(int argc , char *argv[]) {
	
	pid_t pid;
    pid = fork ();
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed\n");
        return 1;
    }
    else if (pid == 0) { /* child process ------- Subcribe host*/
		childProcess();
    }
    else { /* parent process ------------ Admin Container*/
        /* parent will wait for the child to complete*/

        wait(NULL);

		/* SHARED MEMORY */

		/* the size (in bytes) of shared memory object */
    	const int SIZE = 4096;
    	/* name of the shared memory object */
    	const char *name = "OS";
    	/* shared memory file descriptor */
    	int fd;
    	/* pointer to shared memory obect */
    	char *ptr;
    	/* array to receive the data */
    	char data[SIZE];

		fd = shm_open(name, O_RDONLY, 0666);
	    if (fd == -1) {
			perror("open");
			return 10;
		}

		ftruncate (fd, SIZE);

		ptr = (char *) mmap (NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);
    	if (ptr == MAP_FAILED) {
			perror("mmap");
			return 30;
		}
		memcpy(data, ptr, SIZE);
		printf("data received: %s\n", data);

		/*ADMIN CONTAINER*/
		int socket_desc, client_sock, c, read_size;
		struct sockaddr_in server, client;  // https://github.com/torvalds/linux/blob/master/tools/include/uapi/linux/in.h
		char client_message[2000];
		
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
		server.sin_port = htons(7070);
		
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
			puts("Waiting for incoming connections to 7070 ...");
			c = sizeof(struct sockaddr_in);

			//accept connection from an incoming client
			client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
			if(client_sock < 0) {
				perror("accept failed");
			}
			puts("Connection accepted");
			pthread_create(&p, NULL, (void *)acceptingConectionAdmin, &client_sock);
		}
    }

	return 0;
}