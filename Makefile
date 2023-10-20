OBJ_DIR := obj
BIN_DIR := bin
CXX := clang++

VMLINUX := $(OBJ_DIR)/include/vmlinux.h
TRACER_SKEL := $(OBJ_DIR)/include/tracer.skel.h

INCLUDE := common $(OBJ_DIR)/include
INCLUDE_FLAGS := $(patsubst %,-I%,$(INCLUDE))

MAIN := $(BIN_DIR)/main

all: $(MAIN)

$(MAIN): $(TRACER_SKEL) $(VMLINUX)
	@mkdir -p $(dir $@)
	@clang++ -g -std=c++20 -Wno-c99-designator $(INCLUDE_FLAGS) src/main.cpp -lbpf -lelf -o $@

$(OBJ_DIR)/tracer.bpf.o : $(VMLINUX) src/backend/tracer.bpf.c
	@mkdir -p $(dir $@)
# -g flag is really important here, not sure why
	@clang -g -I$(OBJ_DIR)/include -O3 -target bpf -D__TARGET_ARCH_x86_64 -c src/backend/tracer.bpf.c -o obj/tracer.bpf.o 


$(TRACER_SKEL): $(OBJ_DIR)/tracer.bpf.o 
	@mkdir -p $(dir $@)
	@bpftool gen skeleton obj/tracer.bpf.o name tracer > $@

$(VMLINUX):
	@mkdir -p $(dir $@)
	@bpftool btf dump file /sys/kernel/btf/vmlinux format c > $@

.PHONY: clean all
clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR)