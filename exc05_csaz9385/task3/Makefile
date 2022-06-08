DEFINES = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -DMQ_NAME=\"/csaz9385_printmq\" -DBUF_SIZE=8191
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -lrt $(DEFINES)

.PHONY: all
all: print print_server

.PHONY: clean
clean:
	$(RM) -f print print_server

print: task3_printer.c
	$(CC) $(CFLAGS) -o $@ $^

print_server: task3_printer_server.c
	$(CC) $(CFLAGS) -o $@ $^
