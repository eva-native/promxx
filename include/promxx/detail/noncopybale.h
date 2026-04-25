#pragma once

namespace promxx::detail {
class Noncopybale {
public:
  Noncopybale() = default;

  Noncopybale(const Noncopybale &) = delete;
  Noncopybale &operator=(const Noncopybale &) = delete;
};
} // namespace promxx::detail
