#pragma once

#include <atomic>

namespace promxx::metrics {
template <typename T>
class Counter {
public:
  T Inc() { return Inc(T{1}); }
  T Inc(T v) { return value_.fetch_add(v, std::memory_order_relaxed); }

  T Get() const { return value_.load(std::memory_order_relaxed); }

private:
  std::atomic<T> value_{};
};
} // namespace promxx::metrics
