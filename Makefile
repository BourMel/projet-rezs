.PHONY: all
all: server

server: server.c server.h
	gcc server.c -Werror -Wextra -Wall -o server

client: client.c
	gcc client.c -Werror -Wextra -Wall -o client

.PHONY: run-server
run-server: server
	./server ::1 3042

.PHONY: put-client
put-client: client
	./client ::1 3042 put 394c8a052d ::1

.PHONY: get-client
get-client: client
	./client ::1 9000 get 394c8a052d
