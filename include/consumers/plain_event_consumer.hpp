#pragma once

#include "event_consumer.hpp"
#include "event_provider.hpp"

namespace events {
class plain_event_consumer : public event_consumer {
  struct event_visitor {
    bool operator()(fork_event& e);
    bool operator()(exec_event& e);
    bool operator()(exit_event& e);
    bool operator()(write_event& e);
  };

  event_provider& provider;
  pid_t root_pid;
  event_visitor visitor;


 public:
  plain_event_consumer(event_provider& provider, pid_t root_pid);
  int start_consuming() override;

 private:
  bool consume();
};
}  // namespace events