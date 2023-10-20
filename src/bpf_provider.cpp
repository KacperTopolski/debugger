#include <unistd.h>

#include "backend/event.h"
#include "bpf_provider.hpp"
#include <iostream>

bpf_provider::bpf_provider() {
  skel = tracer::open_and_load();
  tracer::attach(skel);
  buffer = ring_buffer__new(bpf_map__fd(skel->maps.queue), buf_process_sample,
                            this, nullptr);
};

bpf_provider::~bpf_provider() {
  tracer::detach(skel);
  tracer::destroy(skel);
};

bool bpf_provider::is_active() { return !tracked_processes.empty() || !messages.empty(); }

std::optional<events::event> bpf_provider::provide() {
  if (messages.empty()) ring_buffer__poll(buffer, 100);
  if (messages.empty()) return {};
  auto result = messages.front();
  messages.pop();
  return {std::move(result)};
}

void bpf_provider::run(char *argv[]) {
  pid_t child = fork();
  int value = 0;

  if (child == 0) {
    pid_t pid = getpid();
    bpf_map__update_elem(skel->maps.processes, &pid, sizeof(pid), &value,
                         sizeof(value), BPF_ANY);
    execvp(argv[0], argv);
    exit(-1);
  } else {
    tracked_processes.insert(child);
  }
}

int bpf_provider::buf_process_sample(void *ctx, void *data, size_t len) {
  bpf_provider *me = (bpf_provider *)ctx;
  event *e = (event *)data;
  events::event new_e;
  switch (e->type) {
    case FORK:
      me->tracked_processes.insert(e->fork.child);
      new_e = events::fork_event{
          {.source_pid = e->fork.parent},
          .child_pid = e->fork.child,
      };
      break;
    case EXIT:
      me->tracked_processes.erase(e->exit.proc);
      new_e = events::exit_event{
          {.source_pid = e->exit.proc},
          .exit_code = 0,
      };
      break;
    case EXEC:
      new_e = events::exec_event{
          {.source_pid = e->exec.proc},
          .uid = 0,
          .command = "",
      };
      break;
    case WRITE:
      std::string data(e->write.size, ' ');
      std::copy(e->write.data, e->write.data + e->write.size, data.begin());
      new_e = events::write_event{
          {.source_pid = e->write.proc},
          .stream = events::write_event::stream::STDOUT,
          .data = std::move(data),
      };
      break;
  }
  me->messages.push(std::move(new_e));
  return 0;
}