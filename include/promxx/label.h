#pragma once

#include <stdexcept>
#include <string>

#include "promxx/detail/string.h"

namespace promxx {
class Label {
public:
  constexpr Label(std::string t_name, std::string t_value)
      : name(std::move(t_name)), value(std::move(t_value)) {
    if (!detail::IsValidLabelName(name))
      throw std::invalid_argument("invalid label name");
    if (!detail::IsValidUTF8(value))
      throw std::invalid_argument("invalid label value");
  }

  const std::string &Name() const noexcept { return name; }
  const std::string &Value() const noexcept { return value; }

  constexpr bool operator==(const Label &) const = default;
  constexpr auto operator<=>(const Label &) const = default;

private:
  std::string name;
  std::string value;
};
} // namespace promxx
