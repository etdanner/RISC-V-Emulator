FROM ubuntu:24.04
RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    gdb \
    make \
    cmake \
    clang \
    clang-format \
    clangd \
    valgrind \
    libasan6 \
    strace \
    ltrace \
    bear \
    git \
    gcc-riscv64-unknown-elf \
    binutils-riscv64-unknown-elf \
    device-tree-compiler \
    && rm -rf /var/lib/apt/lists/*

ARG USERNAME=dev
RUN useradd -ms /bin/bash $USERNAME
USER $USERNAME
WORKDIR /home/$USERNAME