#pragma once

#include <cstddef>
#include <string_view>

namespace promxx::detail {
constexpr bool IsAlpha(char ch) noexcept {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

constexpr bool IsDigit(char ch) noexcept { return ch >= '0' && ch <= '9'; }

constexpr bool IsLabelNameInitChar(char ch) noexcept {
  return IsAlpha(ch) || ch == '_';
}

constexpr bool IsLabelNameChar(char ch) noexcept {
  return IsLabelNameInitChar(ch) || IsDigit(ch);
}

constexpr bool IsMetricNameInitChar(char ch) noexcept {
  return IsLabelNameInitChar(ch) || ch == ':';
}

constexpr bool IsMetricNameChar(char ch) noexcept {
  return IsLabelNameChar(ch) || ch == ':';
}

constexpr bool IsValidLabelName(std::string_view ln) {
  using namespace std::string_view_literals;

  if (ln.empty())
    return false;
  if (!IsLabelNameInitChar(ln[0]))
    return false;
  for (std::size_t idx = 1; idx < ln.size(); ++idx) {
    if (!IsLabelNameChar(ln[idx])) {
      return false;
    }
  }
  if (ln.starts_with("__"sv))
    return false;
  return true;
}

constexpr bool IsValidMetricName(std::string_view mn) {
  if (mn.empty())
    return false;
  if (!IsMetricNameInitChar(mn[0]))
    return false;
  for (std::size_t idx = 1; idx < mn.size(); ++idx) {
    if (!IsMetricNameChar(mn[idx])) {
      return false;
    }
  }
  return true;
}

constexpr bool IsValidUTF8(std::string_view s) {
  std::size_t idx = 0;
  while (idx < s.size()) {
    auto ch = static_cast<unsigned char>(s[idx]);
    std::size_t extra = 0;
    if (ch < 0x80)
      extra = 0;
    else if ((ch & 0xE0) == 0xC0)
      extra = 1;
    else if ((ch & 0xF0) == 0xE0)
      extra = 2;
    else if ((ch & 0xF8) == 0xF0)
      extra = 3;
    else
      return false;
    if (idx + extra >= s.size())
      return false;
    for (std::size_t k = 1; k <= extra; ++k) {
      auto cc = static_cast<unsigned char>(s[idx + k]);
      if ((cc & 0xC0) != 0x80)
        return false;
    }
    idx += extra + 1;
  }
  return true;
}
} // namespace promxx::detail
