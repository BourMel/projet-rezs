.PHONY: all
all: server client

hash.o: hash.c hash.h
	gcc -c hash.c -Werror -Wextra -Wall -o hash.o

server: server.c hash.o
	gcc hash.o server.c -Werror -Wextra -Wall -o server

client: client.c
	gcc hash.o client.c -Werror -Wextra -Wall -o client

.PHONY: run-server
run-server: server
	./server ::1 3042

.PHONY: put-client
put-client: client
	./client ::1 3042 put 394c8a052d ::1

.PHONY: get-client
get-client: client
	./client ::1 9000 get 394c8a052d

# only for some tests
.PHONY: test
test: hash.o test.c
	gcc test.c hash.o -Werror -Wextra -Wall -o test

.PHONY: clean
clean:
	cat .gitignore | xargs -n1 rm -f
