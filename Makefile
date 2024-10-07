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

all: server 

connector.o: connector.c
	$(CC) $(INCLUDES) -c connector.c

server: server.o connector.o
	$(CC) -o server server.o connector.o $(LIBS) $(LDFLAGS)

server.o: server.c
	$(CC) $(INCLUDES) -c server.c

clean: 
	rm -f server server.o connector.o 