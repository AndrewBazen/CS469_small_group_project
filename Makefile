CC := gcc
LDFLAGS := `mysql_config --cflags --libs` -lssl -lcrypto
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
CFLAGS := -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib
# Check if the original CFLAGS work, if not, use Homebrew path
TEST_CFLAGS := $(shell $(CC) $(CFLAGS) -E -xc /dev/null >/dev/null 2>&1 && echo "OK" || echo "FAIL")
ifeq ($(TEST_CFLAGS), FAIL)
CFLAGS := -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib
endif
endif

all: server

server: server.o
	$(CC) $(CFLAGS) -o server server.o $(LDFLAGS)

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean: 
	rm -f server server.o