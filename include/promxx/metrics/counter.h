#pragma once

#include <atomic>
#include <cstdint>

namespace promxx::metrics {
template <typename T = std::uint64_t>
class Counter {
public:
  T Inc() { return Inc(T{1}); }
  T Inc(T v) { return value_.fetch_add(v, std::memory_order_relaxed); }

  T Get() const { return value_.load(std::memory_order_relaxed); }

private:
  std::atomic<T> value_{};
};
} // namespace promxx::metrics
