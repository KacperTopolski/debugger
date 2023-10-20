#pragma once

#include "events.hpp"
#include "event_provider.hpp"

namespace events {
class event_consumer {
 public:
  void consume(event const&);
  virtual ~event_consumer() {}
};
}  // namespace events