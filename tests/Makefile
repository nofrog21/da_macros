CFLAGS := -Wall -Wextra -g
CPP := -I "../"

SRC := $(wildcard *.c)
BIN := $(SRC:.c=)

all: $(BIN)

%: %.c
	$(CC) $(CFLAGS) $(CPP) $< -o $@

run: all
	@for prog in $(BIN); do \
		echo "Running $$prog:"; \
		valgrind --tool=memcheck ./$$prog; \
	done
	$(MAKE) clean
clean:
	rm -f $(BIN)
.PHONY: all run clean
