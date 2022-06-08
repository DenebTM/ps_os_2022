FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
DEFINES = 
CFLAGS = -Wall -Wextra -Wpedantic -Wno-missing-braces -pthread -O3 $(FEATURE_FLAGS) $(DEFINES)
TARGETS = client server

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

client: client.c
	$(CC) $(CFLAGS) -o $@ $^

server: server.c
	$(CC) $(CFLAGS) -o $@ $^