.PHONY: all
all: server

server: server.c server.h
	gcc server.c -Werror -Wextra -Wall -o server

.PHONY: run-server
run-server: server
	./server ::1 3042
