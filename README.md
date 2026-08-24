# RV32I-emu

A RISC-V **RV32I** instruction set emulator written in C. It executes real
RISC-V binaries produced by the standard GNU toolchain and **passes all 40 of
the official `rv32ui` `riscv-tests`**.

The emulator implements the complete RV32I base integer instruction set, a
spec-accurate trap/exception mechanism, a small Linux-style system-call
layer, and it runs programs assembled and linked with `riscv64-unknown-elf-gcc`.

---

## Highlights

- Full **RV32I** base integer instruction set: arithmetic, logic, shifts,
  comparisons, loads/stores, branches, jumps, `lui`/`auipc`, and `ecall`/`ebreak`.
- **40/40** on the official `rv32ui` RISC-V test suite.
- Spec-accurate **trap mechanism** using the real `mcause` exception codes.
- Minimal **Linux syscall ABI** (`exit`, `write`) so programs can print and
  exit with a status code.
- Loads real, linked flat binaries; integrates the official test suite via the
  **HTIF `tohost`** convention.
- Clean, layered C with each concern in its own module.

---

## Building and running

Requires a C compiler and, for building test programs, the RISC-V GNU toolchain
(`riscv64-unknown-elf-gcc`, `binutils`).

```sh
make                                   # build the emulator -> ./emu
make run ARGS="tests/bin/hello.bin"    # run a program
make test-all                          # build + run the full rv32ui suite
```

The emulator takes a single argument: the path to a flat binary to load at
address `0` and execute.

```sh
./emu <program.bin>
```

---

## How it works

The core is a straightforward **fetch → decode → execute** loop over a flat
memory image, with pc starting at the reset vector (`0x0`):

```
load program into memory
initialize CPU (registers zeroed, pc = 0)
loop:
    fetch    32-bit instruction at pc
    decode   raw bits -> fields (opcode, rd, funct3, rs1, rs2, funct7, imm)
    execute  dispatch on opcode/funct3/funct7, mutate CPU + memory
    handle   any trap the instruction returned (ebreak, ecall, fault, tohost)
```

Every instruction returns a `trap_cause_t`. `TRAP_NONE` means it executed
cleanly; anything else would be an illegal instruction, a bad memory access, an
`ecall`, or an `ebreak`. These traps flow up to the main loop, where it decides what to
do with it. This keeps the execution core uniform: the loop main control loop, not the
individual instructions, owns control-flow decisions like halting.

---

## Architecture

The code is split by concern, with dependencies pointing in one direction so
there are no cycles.

| Module        | Responsibility                                                        |
|---------------|-----------------------------------------------------------------------|
| `cpu`         | Processor state (`regs[32]`, `pc`) and its primitive operations.       |
| `memory`      | The address space. Bounds/alignment-checked byte access, private array.|
| `decode`      | Pure function: 32 raw bits in, a `decoded_instr_t` of fields out.      |
| `execute`     | Instruction semantics. Dispatches on the decoded fields.               |
| `syscall`     | The guest↔host boundary: `ecall` handling (`exit`, `write`).           |
| `trap`        | Shared exception cause codes.                                          |
| `main`        | Program loading, the fetch-decode-execute loop, trap routing.          |


`decode` and `memory` are self-contained and independently testable.
`execute` is the hub where the other pieces meet and everything flows toward
`main`.

---

## Design decisions

A few choices that shaped the implementation, and why.

### Decode and execute are separate stages

`decode` is a pure function: raw instruction word in, a struct of extracted
fields out, no CPU state touched. `execute` consumes that struct and applies the
semantics. This mirrors how a real pipeline separates the stages and makes
decode trivially unit-testable: a given 32-bit word must always produce the same
fields, independent of machine state. There is deliberately no separate
"classify the instruction" step as the combination of `opcode`/`funct3`/`funct7` already
*is* the instruction's identity, so `execute` switches on it directly rather
than translating to an intermediate enum.

### Memory is encapsulated behind typed accessors

The memory array is `static` inside the `memory` module and never exposed. The
public interface is a set of width-specific functions (`mem_read8/16/32`,
`mem_write8/16/32`, `mem_fetch32`). Centralizing the bounds check, alignment rules,
and little-endian byte assembly within the mem functions helps avoid copy-paste bugs,
while the typed memory access wrappers make it impossible to request an invalid access
width from outside the module.

### Traps are a value

RISC-V exceptions (misaligned access, access fault, illegal instruction,
`ecall`, `ebreak`) are modeled as the return value of every instruction, using
the actual `mcause` cause codes from the privileged spec. The number the
emulator returns *is* the number that would land in the `mcause` register. This
keeps the memory layer ignorant of *why* it's being accessed. Functions return a
generic fault, and the caller maps it to the load/store/fetch-specific cause keeping
guest exceptions cleanly separated from host-side errors (a bad ROM file, a failed allocation),
which are handled with ordinary C error handling in `main`.

### Sign vs. zero extension lives in `execute`, not `memory`

Memory moves bytes; it always zero-extends what it reads into a 32-bit word.
Whether a loaded byte is *sign*-extended (`lb`, `lh`) or *zero*-extended (`lbu`,
`lhu`) is a property of the instruction, so it is decided in `execute` via a cast
through the appropriate narrow signed type. This keeps each concern where it
belongs and avoids leaking instruction semantics into the memory subsystem.

---

## Testing

The emulator is validated two ways.

**Hand-written programs** in `tests/asm/` exercise specific behaviors
(load/store round-trips, sign-extension corners, loops, function call/return via
the link register). These are assembled and linked with the GNU toolchain and
run directly.

**The official RISC-V test suite** (`rv32ui`) is the authoritative check. Those
tests exercise every instruction against a reference implementation, including
the edge cases hand-written tests tend to miss: signed-overflow boundaries,
shift-by-0 and shift-by-31, sign/zero-extension corners, source/destination
register aliasing, and the scrambled branch/jump immediate encodings.

The tests report their result using the HTIF **`tohost`** convention: a store
to a fixed memory address (`0x1000`) whose value encodes pass/fail. The emulator
watches for that store, halts, and returns the result as its process exit code
(`0` = pass, otherwise the failing sub-test number), which lets the whole suite
be run and tallied automatically:

```sh
make test-all
```

```
PASS add
PASS addi
...
PASS xor
PASS xori
-----------------------------
passed: 40  failed: 0
```

Two suite tests are intentionally excluded: `fence_i` (requires `fence.i` and
self-modifying-code support) and `ma_data` (requires a misaligned-access trap
handler). Both are on the roadmap.

---

## Toolchain

Test programs are built with the standard flow:

```
riscv64-unknown-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -nostartfiles \
    -I tests/riscv -T link.ld <test>.S -o <test>.elf
riscv64-unknown-elf-objcopy -O binary <test>.elf <test>.bin
```

A custom linker script (`link.ld`) places code at address `0` to match the
emulator's load address and reset vector, and pins the `tohost` region so the
test harness and the emulator agree on where completion is signaled. The build
is wired into the `Makefile` via pattern rules, so adding a test is just
dropping a new source file in the appropriate directory.

---

## Future Plans

- **M extension** — integer multiply/divide (`mul`, `div`, ...).
- **CSRs + full trap handling** — `csrr`/`csrw`, `mtvec`/`mepc`/`mcause`, and a
  trap vector, which would enable the `fence_i` and `ma_data` suite tests.
- **A companion assembler** — inverting the decoder to turn RV32I assembly back
  into machine code, cross-validated against the emulator.
- **Demo programs** — small self-contained programs (Fibonacci, sorting) that
  produce output via the `write` syscall.

---

## Project layout

```
src/            emulator source (cpu, memory, decode, execute, syscall, main)
tests/asm/      hand-written assembly tests
tests/riscv/    official riscv-tests sources + minimal test environment
link.ld         linker script (load at 0x0, tohost region)
Makefile        build, run, and test targets
```
