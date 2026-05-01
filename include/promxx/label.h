#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace promxx {
class Label {
public:
  Label() = default;

  Label(std::string name, std::string value)
      : name_(std::move(name)), value_(std::move(value)) {}

  bool Empty() const noexcept;

  const std::string &Name() const noexcept { return name_; }
  const std::string &Value() const noexcept { return value_; }

  constexpr friend auto operator<=>(const Label &, const Label &) = default;
  constexpr friend bool operator==(const Label &, const Label &) = default;

private:
  void Validate();

  std::string name_;
  std::string value_;
};

class LabelSet {
public:
  using ContainerType = std::vector<Label>;

  LabelSet() = default;
  LabelSet(std::initializer_list<Label> label);

  void InsertOrAssign(Label label);
  bool Erase(std::string_view name);

  [[nodiscard]]
  std::span<const Label> View() const noexcept;

  constexpr friend auto operator<=>(const LabelSet &, const LabelSet &) = default;
  constexpr friend bool operator==(const LabelSet &, const LabelSet &) = default;

private:
  [[nodiscard]]
  ContainerType::iterator Find(std::string_view name);

  ContainerType labels_;
};
} // namespace promxx
