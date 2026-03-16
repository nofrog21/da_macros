CFLAGS := -Wall -Wextra -fstrict-aliasing -Wstrict-aliasing -g
ASANFLAGS := -fsanitize=alignment,null,address
CPP := -I "../"

SRC := $(wildcard *.c)
BIN := $(SRC:.c=)
BINASAN :=$(SRC:.c=_asan)

all: $(BIN)

%: %.c
	$(CC) $(CFLAGS) $(CPP) $< -o $@
%_asan: %.c
	$(CC) $(CFLAGS) $(ASANFLAGS) $(CPP) $< -o $@

run: $(BINASAN) $(BIN)
	@for prog in $(BIN); do \
		echo "Running valgrind with $$prog:"; \
		valgrind --tool=memcheck --leak-check=full ./$$prog; \
	done
	@for prog in $(BINASAN); do \
		echo "Running $$prog:"; \
		./$$prog; \
	done
	$(MAKE) clean
clean:
	rm -f $(BIN) $(BINASAN)
.PHONY: all run clean
