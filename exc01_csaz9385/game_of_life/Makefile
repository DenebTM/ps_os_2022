CFLAGS = -std=c11 -Wall -Wextra -Wpedantic #-fsanitize=address -Wconversion

.PHONY: all
all: game

.PHONY: clean
clean:
	$(RM) -rf game out gol.gif

game: game.c
	$(CC) $(CFLAGS) -o $@ $^
