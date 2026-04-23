#pragma once

#include <atomic>
#include <cstdint>

#include "promxx/meta/concepts.h"

namespace promxx::metrics {
template <typename T = std::uint64_t>
  requires(MetricValue<T>)
class Counter {
public:
  void Inc() { Inc(T{1}); }
  void Inc(T v) { value_.fetch_add(v, std::memory_order_relaxed); }

  T Get() const { return value_.load(std::memory_order_relaxed); }

private:
  std::atomic<T> value_{};
};
} // namespace promxx::metrics
