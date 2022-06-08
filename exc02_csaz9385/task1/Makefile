CFLAGS = -std=c11 -Wall -Wextra -Wpedantic

.PHONY: all
all: read_my_env_var

.PHONY: clean
clean:
	$(RM) -f read_my_env_var

read_my_env_var: read_my_env_var.c
	$(CC) $(CFLAGS) -o $@ $^
