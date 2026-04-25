#pragma once

#include "promxx/metrics/description.h"
#include "promxx/metrics/family_base.h"

namespace promxx::metrics {
template <typename M>
class Family : public FamilyBase {
public:
  explicit Family(Description desc) : FamilyBase(std::move(desc)) {}

  M &Add() {

  }

private:
};
} // namespace promxx::metrics
