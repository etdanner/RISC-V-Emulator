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

# List of rv32ui tests to run (excludes fence_i and ma_data which need trap/CSR support)
RVTESTS := add addi and andi auipc beq bge bgeu blt bltu bne \
           jal jalr lb lbu lh lhu lui lw or ori sb sh simple \
           sll slli slt slti sltiu sltu sra srai srl srli sub sw xor xori \
           ld_st st_ld

test-all: $(TARGET)
	@pass=0; fail=0; \
	for t in $(RVTESTS); do \
		if $(MAKE) -s tests/bin/$$t.bin >/dev/null 2>&1; then \
			if ./$(TARGET) tests/bin/$$t.bin >/dev/null 2>&1; then \
				echo "PASS $$t"; pass=$$((pass+1)); \
			else \
				echo "FAIL $$t (exit $$?)"; fail=$$((fail+1)); \
			fi; \
		else \
			echo "BUILD-FAIL $$t"; fail=$$((fail+1)); \
		fi; \
	done; \
	echo "-----------------------------"; \
	echo "passed: $$pass  failed: $$fail"

.PHONY: clean debug run test test-all
