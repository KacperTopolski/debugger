#include "consumers/plain_event_consumer.hpp"

#include <iomanip>
#include <iostream>

#include "spdlog/spdlog.h"

using namespace events;

plain_event_consumer::plain_event_consumer(event_provider& provider,
                                           pid_t root_pid)
    : provider(provider), root_pid(root_pid) {}

int plain_event_consumer::start_consuming() {
  SPDLOG_INFO("Starting plain consumer");
  while (consume())
    ;
  return 0;
}

uint64_t nanoseconds(time_point timestamp) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             timestamp.time_since_epoch())
      .count();
}

bool plain_event_consumer::consume() {
  std::optional<event> provided_event = provider.provide();
  if (!provided_event.has_value()) return true;
  event e = provided_event.value();
  std::visit(visitor, e);
  return true;
}

bool plain_event_consumer::event_visitor::operator()(fork_event& e) {
  std::cout << std::setw(20) << e.timestamp << "\n";
  return true;
}

bool plain_event_consumer::event_visitor::operator()(exec_event& e) {
  if (e.command.size() > 0)
    std::cout << std::setw(20) << nanoseconds(e.timestamp) << "\n";
  return true;
}

bool plain_event_consumer::event_visitor::operator()(exit_event& e) {
  return true;
}

bool plain_event_consumer::event_visitor::operator()(write_event& e) {
  if (e.data.size() > 0)
    std::cout << std::setw(20) << nanoseconds(e.timestamp) << "\n";
  return true;
}

std::string stream_name(enum write_event::stream s) {
  return s == write_event::stream::STDOUT ? "STDOUT" : "STDERR";
}
