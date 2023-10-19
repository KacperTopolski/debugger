all: vmlinux.h
	@mkdir -p obj
	@clang -g -O3 -target bpf -D__TARGET_ARCH_x86_64 -c src/backend/tracer.bpf.c -o obj/tracer.bpf.o
	@bpftool gen skeleton obj/tracer.bpf.o name tracer > src/backend/tracer.skel.h
	@clang++ -std=c++20 -Wno-c99-designator -Icommon src/main.cpp -lbpf -lelf -o debugger

vmlinux.h:
	@bpftool btf dump file /sys/kernel/btf/vmlinux format c > src/backend/vmlinux.h

.PHONY: clean all
clean:
	@rm tracer.skel.h vmlinux.h tracer *.o