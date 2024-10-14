CC := gcc
LDFLAGS := -lssl -lcrypto -lmysqlclient
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
OPENSSL_PATH := /opt/homebrew/opt/openssl@3
REPOSITORY_PATH := ./repository
MYSQL_PATH := /opt/homebrew/opt/mysql
INCLUDES := -I$(OPENSSL_PATH)/include -I$(MYSQL_PATH)/include -I$(REPOSITORY_PATH)
LIBS := -L$(OPENSSL_PATH)/lib -L$(MYSQL_PATH)/lib
else
OPENSSL_PATH := /usr
MYSQL_PATH := /usr
REPOSITORY_PATH := ./repository
INCLUDES := -I$(OPENSSL_PATH)/include -I$(MYSQL_PATH)/include/mysql -I$(REPOSITORY_PATH)
LIBS := -L$(OPENSSL_PATH)/lib -L$(MYSQL_PATH)/lib
endif

# List of object files in the repository directory
REPOSITORY_OBJS := $(patsubst $(REPOSITORY_PATH)/%.c, $(REPOSITORY_PATH)/%.o, $(wildcard $(REPOSITORY_PATH)/*.c))

all: server repl

# Compile each .c file in the repository directory to a .o file
$(REPOSITORY_PATH)/%.o: $(REPOSITORY_PATH)/%.c
	$(CC) $(INCLUDES) -c $< -o $@

connector.o: connector.c
	$(CC) $(INCLUDES) -c connector.c

repl: repl.o connector.o
	$(CC) -o repl repl.o connector.o $(LIBS) $(LDFLAGS)

repl.o: repl.c
	$(CC) $(INCLUDES) -c repl.c

utilities.o: utilities.c
	$(CC) $(INCLUDES) -c utilities.c

server: server.o connector.o utilities.o request.o $(REPOSITORY_OBJS)
	$(CC) -o server server.o connector.o utilities.o request.o $(REPOSITORY_OBJS) $(LIBS) $(LDFLAGS)

server.o: server.c
	$(CC) $(INCLUDES) -c server.c

request.o: request.c
	$(CC) $(INCLUDES) -c request.c

clean: 
	rm -f server server.o connector.o repl repl.o utilities.o request.o $(REPOSITORY_OBJS)