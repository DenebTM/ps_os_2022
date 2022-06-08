CC = gcc
FEATURE_FLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700
CFLAGS = -Wall -Wextra -pthread -std=c11 $(FEATURE_FLAGS)

PLUGINS = double.so square.so sqrt.so binary.so
TARGETS = map_number $(PLUGINS)

.PHONY: all
all: clean $(TARGETS)

.PHONY: clean
clean:
	$(RM) -f $(TARGETS)

map_number: map_number.c
	$(CC) $(CFLAGS) -ldl -o $@ $^ -g

plugins: $(PLUGINS)
define plugin_recipe
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ -g
endef
double.so: double.c ; $(plugin_recipe)
square.so: square.c ; $(plugin_recipe)
sqrt.so:   sqrt.c   ; $(plugin_recipe) -lm
binary.so: binary.c ; $(plugin_recipe)
