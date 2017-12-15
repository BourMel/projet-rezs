.PHONY: all
all: server client

dht.o: dht.c dht.h
	gcc -c dht.c -Werror -Wextra -Wall -o dht.o

server: server.c dht.o
	gcc dht.o server.c -Werror -Wextra -Wall -o server

client: client.c
	gcc dht.o client.c -Werror -Wextra -Wall -o client

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
test: dht.o test.c
	gcc test.c dht.o -Wextra -Wall -o test
.PHONY: test_client
test_client: dht.o test_client.c
	gcc test_client.c dht.o -Wextra -Wall -o test_client
.PHONY: test_server
test_server: dht.o test_server.c
	gcc test_server.c dht.o -Wextra -Wall -o test_server

.PHONY: clean
clean:
	cat .gitignore | xargs -n1 rm -f
