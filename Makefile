CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -g -O0 -MMD -MP
TARGET   := emu

SRC      := $(wildcard src/*.c)
OBJ      := $(SRC:.c=.o)
DEP      := $(OBJ:.o=.d)
RVAS    := riscv64-unknown-elf-as
RVCPY   := riscv64-unknown-elf-objcopy
RVFLAGS := -march=rv32i -mabi=ilp32

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEP)

debug: CFLAGS += -fsanitize=address,undefined
debug: clean $(TARGET)

clean:
	rm -f src/*.o src/*.d $(TARGET)
	rm -rf tests/bin

run: $(TARGET)
	./$(TARGET) $(ARGS)

test: tests/bin/testing.bin $(TARGET)
	./$(TARGET) tests/bin/testing.bin

RVAS    := riscv64-unknown-elf-as
RVLD    := riscv64-unknown-elf-ld
RVCPY   := riscv64-unknown-elf-objcopy
RVFLAGS := -march=rv32i -mabi=ilp32
LDSCRIPT := link.ld

tests/bin/%.bin: tests/asm/%.s $(LDSCRIPT)
	@mkdir -p tests/bin
	$(RVAS) $(RVFLAGS) $< -o $@.o
	$(RVLD) -T $(LDSCRIPT) -melf32lriscv $@.o -o $@.elf
	$(RVCPY) -O binary $@.elf $@
	@rm -f $@.o $@.elf

tests/bin/%.bin: tests/riscv/rv32ui/%.S link.ld
	@mkdir -p tests/bin
	riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles \
	    -I tests/riscv -T link.ld $< -o $@.elf
	riscv64-unknown-elf-objcopy -O binary $@.elf $@
	@rm -f $@.elf

.PHONY: clean debug run test
