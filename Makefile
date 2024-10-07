CC := gcc
LDFLAGS := -lssl -lcrypto -lmysqlclient
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
OPENSSL_PATH := /opt/homebrew/opt/openssl@3
MYSQL_PATH := /opt/homebrew/opt/mysql
INCLUDES := -I$(OPENSSL_PATH)/include -I$(MYSQL_PATH)/include
LIBS := -L$(OPENSSL_PATH)/lib -L$(MYSQL_PATH)/lib
else
endif

all: server repl

connector.o: connector.c
	$(CC) $(INCLUDES) -c connector.c

repl: repl.o connector.o
	$(CC) -o repl repl.o connector.o $(LIBS) $(LDFLAGS)

repl.o: repl.c
	$(CC) $(INCLUDES) -c repl.c

utilities.o: utilities.c
	$(CC) $(INCLUDES) -c utilities.c

server: server.o connector.o utilities.o request.o
	$(CC) -o server server.o connector.o utilities.o request.o $(LIBS) $(LDFLAGS)

server.o: server.c
	$(CC) $(INCLUDES) -c server.c

request.o: request.c
	$(CC) $(INCLUDES) -c request.c

clean: 
	rm -f server server.o connector.o repl repl.o utilities.o request.o