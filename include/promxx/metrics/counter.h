#pragma once

#include <atomic>
#include <cstdint>

#include "promxx/detail/noncopybale.h"
#include "promxx/meta/concepts.h"

namespace promxx::metrics {
template <typename T>
  requires(MetricValue<T>)
class CounterBase : ::promxx::detail::Noncopybale {
public:
  void Inc() { Inc(T{1}); }
  void Inc(T v) { value_.fetch_add(v, std::memory_order_relaxed); }

  T Get() const { return value_.load(std::memory_order_relaxed); }

private:
  std::atomic<T> value_{};
};

using Counter = CounterBase<std::uint64_t>;
} // namespace promxx::metrics
