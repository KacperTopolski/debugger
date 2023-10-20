#include "consumers/plain_event_consumer.hpp"

#include <iomanip>
#include <iostream>

using namespace events;

uint64_t nanoseconds(time_point timestamp) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             timestamp.time_since_epoch())
      .count();
}

void plain_event_consumer::consume(event const& e) {
  std::visit(visitor, e);
}

std::string unescape(std::string const& s)
{
  std::string result;
  for (char c : s)
    if (c == '\n')
      result += "\\n";
    else
      result.push_back(c);
  return result;
}

void plain_event_consumer::event_visitor::operator()(fork_event const& e) {
    std::cout 
    << std::setw(30) << e.timestamp 
    << std::setw(8) << e.source_pid 
    << std::setw(6) << "FORK" 
    << std::setw(8) << e.child_pid
    << "\n";
}

void plain_event_consumer::event_visitor::operator()(exec_event const& e) {
    std::cout 
    << std::setw(30) << e.timestamp 
    << std::setw(8) << e.source_pid 
    << std::setw(6) << "EXEC" 
    << std::setw(8) << e.uid
    << " " << unescape(e.command)
    << "\n";
}

void plain_event_consumer::event_visitor::operator()(exit_event const& e) {
    std::cout 
    << std::setw(30) << e.timestamp 
    << std::setw(8) << e.source_pid 
    << std::setw(6) << "EXIT"
    << " " << e.exit_code
    << "\n";
}

std::string stream_name(enum write_event::stream s) {
  return s == write_event::stream::STDOUT ? "STDOUT" : "STDERR";
}

void plain_event_consumer::event_visitor::operator()(write_event const& e) {
    std::cout 
    << std::setw(30) << e.timestamp 
    << std::setw(8) << e.source_pid 
    << std::setw(6) << "WRITE"
    << " " << stream_name(e.stream)
    << " " << unescape(e.data)
    << "\n";
}

