#pragma once

#include <atomic>

#include "promxx/meta/concepts.h"

namespace promxx::metrics {
template <typename T = double>
  requires(MetricValue<T>)
class Gauge {
public:
  void Inc() { Inc(T{1}); }
  void Inc(T v) { value_.fetch_add(v, std::memory_order_relaxed); }

  void Dec() { Dec(T{1}); }
  void Dec(T v) { value_.fetch_sub(v, std::memory_order_relaxed); }

  void Set(T v) { value_.store(v, std::memory_order_relaxed); }
  T Get() const { return value_.load(std::memory_order_relaxed); }

private:
  std::atomic<T> value_{};
};
} // namespace promxx::metrics
