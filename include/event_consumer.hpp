#pragma once

#include "events.hpp"
#include "event_provider.hpp"

namespace events {
class event_consumer {
 public:
  virtual int start_consuming() = 0;
  virtual ~event_consumer() {}
};
}  // namespace events