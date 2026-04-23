#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

namespace promxx::metrics {

template <typename T = double>
class Histogram {
public:
  explicit Histogram(std::vector<double> bounds) : bounds_(std::move(bounds)) {
    if (bounds_.empty()) {
      throw std::invalid_argument("bounds must not be empty");
    }
    if (bounds_.back() != std::numeric_limits<double>::infinity()) {
      bounds_.push_back(std::numeric_limits<double>::infinity());
    }
    bucket_counts_ =
        std::make_unique<std::atomic<std::uint64_t>[]>(bounds_.size());
  }

  void Observe(T value) {
    auto it = std::lower_bound(bounds_.begin(), bounds_.end(),
                               static_cast<double>(value));
    bucket_counts_[static_cast<std::size_t>(it - bounds_.begin())].fetch_add(
        1, std::memory_order_relaxed);
    sum_.fetch_add(value, std::memory_order_relaxed);
    count_.fetch_add(1, std::memory_order_relaxed);
  }

  [[nodiscard]] std::uint64_t GetCount() const {
    return count_.load(std::memory_order_relaxed);
  }

  [[nodiscard]] T GetSum() const {
    return sum_.load(std::memory_order_relaxed);
  }

  [[nodiscard]] std::vector<std::uint64_t> GetBuckets() const {
    std::vector<std::uint64_t> result;
    result.reserve(bounds_.size());
    std::uint64_t cumulative = 0;
    for (std::size_t i = 0; i < bounds_.size(); ++i) {
      cumulative += bucket_counts_[i].load(std::memory_order_relaxed);
      result.push_back(cumulative);
    }
    return result;
  }

  [[nodiscard]] const std::vector<double> &GetBounds() const { return bounds_; }

private:
  std::vector<double> bounds_;
  std::unique_ptr<std::atomic<std::uint64_t>[]> bucket_counts_;
  std::atomic<std::uint64_t> count_{0};
  std::atomic<T> sum_{T{}};
};

constexpr inline std::vector<double> DefaultBuckets() {
  return {.005, .01, .025, .05, .1, .25, .5, 1.0, 2.5, 5.0, 10.0};
}

constexpr inline std::vector<double> LinearBuckets(double start, double width,
                                                   int count) {
  if (count <= 0)
    throw std::invalid_argument("count must be positive");
  std::vector<double> bounds;
  bounds.reserve(static_cast<std::size_t>(count));
  for (int i = 0; i < count; ++i) {
    bounds.push_back(start + width * i);
  }
  return bounds;
}

constexpr inline std::vector<double>
ExponentialBuckets(double start, double factor, int count) {
  if (count <= 0)
    throw std::invalid_argument("count must be positive");
  if (start <= 0)
    throw std::invalid_argument("start must be positive");
  if (factor <= 1)
    throw std::invalid_argument("factor must be greater than 1");
  std::vector<double> bounds;
  bounds.reserve(static_cast<std::size_t>(count));
  for (int i = 0; i < count; ++i) {
    bounds.push_back(start * std::pow(factor, i));
  }
  return bounds;
}

} // namespace promxx::metrics
