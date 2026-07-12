CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -g -O0 -MMD -MP
TARGET   := emu

SRC      := $(wildcard src/*.c)
OBJ      := $(SRC:.c=.o)
DEP      := $(OBJ:.o=.d)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $

-include $(DEP)

debug: CFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	rm -f src/*.o src/*.d $(TARGET)

.PHONY: clean debug
