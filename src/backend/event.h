#pragma once 

enum event_type {
    FORK,
    EXIT,
    EXEC,
    WRITE
};

struct fork_event {
    enum event_type type;
    unsigned long long timestamp;
    pid_t parent;
    pid_t child;
};

struct exec_event {
    enum event_type type;
    unsigned long long timestamp;
    pid_t proc;
};

struct exit_event {
    enum event_type type;
    unsigned long long timestamp;
    pid_t proc;
};

struct write_event {
    enum event_type type;
    unsigned long long timestamp;
    pid_t proc;
    int size;
    char data[];
};

union event {
    struct {
        enum event_type type;
        unsigned long long timestamp;
    };
    struct fork_event fork;
    struct exec_event exec;
    struct exit_event exit;
    struct write_event write;
};

#ifndef __cplusplus
static inline void make_fork_event(struct fork_event *event, pid_t parent, pid_t child) {
    event->type = FORK;
    event->timestamp = bpf_ktime_get_ns();
    event->parent = parent;
    event->child = child;
}

static inline void make_exit_event(struct exit_event *event, pid_t proc) {
    event->type = EXIT;
    event->timestamp = bpf_ktime_get_ns();
    event->proc = proc;
}

static inline void make_exec_event(struct exec_event *event, pid_t proc) {
    event->type = EXEC;
    event->timestamp = bpf_ktime_get_ns();
    event->proc = proc;
}

static inline void make_write_event(struct write_event *event, pid_t proc, int size) {
    event->type = WRITE;
    event->timestamp = bpf_ktime_get_ns();
    event->proc = proc;
    event->size = size;
}
#endif