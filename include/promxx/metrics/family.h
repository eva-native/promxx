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
    if (metrics_.contains(labels))
      throw std::invalid_argument("duplicate metric labels");
    metrics_[labels] = M{};
  }

private:
  std::map<LabelSet, M> metrics_;
};
} // namespace promxx::metrics
