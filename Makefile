CC=gcc

%.o: %.c
	$(CC) -c -o $@ $<
allmessage: message_sender01 message_sender02 message_receiver message_sender03_multiplemessages message_receiver02_multiplemessages.c
allnamepipes: pipe_writer01 pipe_writer02 pipe_reader01 pipe_reader02 ordinary_pipe01 ordinary_pipe02
all: allmessage shared_memory_set01 shared_memory_get01 allnamepipes socket_client socket_server socket_server_main socket_server_host1 socket_server_host2

shared_memory_set01: shared_memory_set01.o
	gcc -o shared_memory_set01 shared_memory_set01.o -lrt

shared_memory_get01: shared_memory_get01.o
	gcc -o shared_memory_get01 shared_memory_get01.o -lrt

shared_memory_set02: shared_memory_set02.o
	gcc -o shared_memory_set02 shared_memory_set02.o -lrt

shared_memory_get02: shared_memory_get02.o
	gcc -o shared_memory_get02 shared_memory_get02.o -lrt

message_sender01: message_sender01.o
	gcc -o message_sender01 message_sender01.o -lrt

message_sender02: message_sender02.o
	gcc -o message_sender02 message_sender02.o -lrt

message_receiver: message_receiver.o
	gcc -o message_receiver message_receiver.o -lrt

message_sender03_multiplemessages: message_sender03_multiplemessages.o
	gcc -o message_sender03_multiplemessages message_sender03_multiplemessages.o -lrt

message_receiver02_multiplemessages: message_receiver02_multiplemessages.o
	gcc -o message_receiver02_multiplemessages message_receiver02_multiplemessages.o -lrt

pipe_writer01: pipe_writer01.o
	gcc -o pipe_writer01 pipe_writer01.o

pipe_writer02: pipe_writer02.o
	gcc -o pipe_writer02 pipe_writer02.o

pipe_reader01: pipe_reader01.o
	gcc -o pipe_reader01 pipe_reader01.o

pipe_reader02: pipe_reader02.o
	gcc -o pipe_reader02 pipe_reader02.o

ordinary_pipe01: ordinary_pipe01.o
	gcc -o ordinary_pipe01 ordinary_pipe01.o

ordinary_pipe02: ordinary_pipe02.o
	gcc -o ordinary_pipe02 ordinary_pipe02.o

socket_client: socket_client.o
	gcc -o socket_client socket_client.o
	
socket_server: socket_server.o
	gcc -o socket_server socket_server.o

socket_server_main: socket_server_main.o
	gcc -pthread -o socket_server_main socket_server_main.o -lrt

socket_server_host1: socket_server_host1.o
	gcc -pthread -o socket_server_host1 socket_server_host1.o -lrt

socket_server_host2: socket_server_host2.o
	gcc -pthread -o socket_server_host2 socket_server_host2.o -lrt

tar:
	tar cf ../examples.tar .
	mv ../examples.tar .

clean:
	rm -f *.o
	rm -f *.class
	rm -f message_sender01 message_sender02 message_receiver shared_memory_get01 shared_memory_get02 shared_memory_set01 shared_memory_set02
	rm -f ordinary_pipe01 ordinary_pipe02 pipe_reader01 pipe_writer01 pipe_reader02 pipe_writer02
	rm -f socket_client socket_server