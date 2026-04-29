#include "promxx/label.h"

#include <algorithm>
#include <stdexcept>
#include <string_view>

#include "promxx/detail/string.h"

namespace promxx {
void Label::Validate() {
  if (detail::IsValidLabelName(name_)) {
    throw std::invalid_argument("invalid label name");
  }
}

bool Label::Empty() const noexcept { return value_.empty(); }

void LabelSet::InsertOrAssign(Label label) {
  auto it = Find(label.Name());

  if (it != labels_.end() && it->Name() == label.Name()) {
    *it = std::move(label);
  } else {
    labels_.insert(it, std::move(label));
  }
}

bool LabelSet::Erase(std::string_view name) {
  auto it = Find(name);

  if (it != labels_.end() && it->Name() == name) {
    labels_.erase(it);
    return true;
  }
  return false;
}

std::span<const Label> LabelSet::View() const noexcept {
  return {labels_.data(), labels_.size()};
}

LabelSet::ContainerType::iterator LabelSet::Find(std::string_view name) {
  return std::lower_bound(labels_.begin(), labels_.end(), name,
                          [](const Label &item, std::string_view target) {
                            return item.Name() < target;
                          });
}
} // namespace promxx
