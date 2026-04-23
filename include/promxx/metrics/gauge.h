#pragma once

#include <atomic>

namespace promxx::metrics {
template <typename T>
class Gauge {
public:
  T Inc() { return Inc(T{1}); }
  T Inc(T v) { return value_.fetch_add(v, std::memory_order_relaxed); }

  T Dec() { return Dec(T{1}); }
  T Dec(T v) { return value_.fetch_sub(v, std::memory_order_relaxed); }

  T Set(T v) { return value_.exchange(v); }
  T Get() const { return value_.load(std::memory_order_relaxed); }

private:
  std::atomic<T> value_;
};
} // namespace promxx::metrics
