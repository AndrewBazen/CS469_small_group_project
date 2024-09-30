CC := gcc
LDFLAGS := `mysql_config --cflags --libs` #-lssl -lcrypto add crypto flags later when needed
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CFLAGS := # -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib
endif

all: server

server: server.o
	$(CC) $(CFLAGS) -o server server.o $(LDFLAGS)

server.o: server.c
	$(CC) $(CFLAGS) -c server.c