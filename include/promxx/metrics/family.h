#pragma once

#include <map>
#include <stdexcept>

#include "promxx/label.h"
#include "promxx/metrics/description.h"
#include "promxx/metrics/family_base.h"

namespace promxx::metrics {
template <typename M>
class Family : public FamilyBase {
public:
  explicit Family(Description desc) : FamilyBase(std::move(desc)) {}

  M &Add(LabelSet labels) {
    auto [it, ok] = metrics_.try_emplace(std::move(labels));
    if (!ok) {
      throw std::invalid_argument("duplicate metric labelset");
    }
    return it->second;
  }

private:
  std::map<LabelSet, M> metrics_;
};
} // namespace promxx::metrics
