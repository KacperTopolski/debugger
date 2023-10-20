#include <bpf/bpf.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <string>

#include "bpf_provider.hpp"
#include "tracer.skel.h"

bool has_children() {
  pid_t err;
  while ((err = waitpid(-1, nullptr, WNOHANG)) > 0)
    ;
  return err == 0;
}

static void increase_memlock_limit() {
  rlimit lim{
      .rlim_cur = RLIM_INFINITY,
      .rlim_max = RLIM_INFINITY,
  };
  if (setrlimit(RLIMIT_MEMLOCK, &lim))
    throw "Failed to increase RLIMIT_MEMLOCK\n";
}

int main(int argc, char *argv[]) {
  increase_memlock_limit();

  bpf_provider provider;
  provider.run(argv + 1);
  while (provider.is_active()) {
    auto v = provider.provide();
    if (v.has_value()) {
      auto event = v.value();
      if (std::holds_alternative<events::exec_event>(event)) {
        std::cout << std::get<events::exec_event>(event).source_pid
                  << std::endl;
      }
    }
  }
  return 0;
}