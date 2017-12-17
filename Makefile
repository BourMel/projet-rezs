.PHONY: all
all: server client

dht.o: dht.c dht.h
	gcc -c dht.c -Werror -Wextra -Wall -o dht.o

.PHONY: run-server
run-server: server
	./server ::1 1234

.PHONY: put-client
put-client: client
	./client ::1 1234 put 8NmV2C3vdRN5K2ShsdU8TZUaG04frUFTB7w1mQYndD1TT6gY88NfbhTchRTwiCYAMHH2PJKK ::1

.PHONY: get-client
get-client: client
	./client ::1 1234 get 8NmV2C3vdRN5K2ShsdU8TZUaG04frUFTB7w1mQYndD1TT6gY88NfbhTchRTwiCYAMHH2PJKK

client: dht.o client.c
	gcc client.c dht.o -pthread -Werror -Wextra -Wall -o client

server: dht.o server.c
	gcc server.c dht.o -Werror -Wextra -Wall -o server

.PHONY: clean
clean:
	cat .gitignore | xargs -n1 rm -f
