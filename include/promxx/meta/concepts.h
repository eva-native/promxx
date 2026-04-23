#pragma once

#include <concepts>

namespace promxx {
template <typename T>
concept MetricValue = std::integral<T> || std::floating_point<T>;
}
