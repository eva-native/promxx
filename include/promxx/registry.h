#pragma once

#include <memory>
#include <vector>

#include "promxx/metrics/description.h"
#include "promxx/metrics/family.h"
#include "promxx/metrics/family_base.h"

namespace promxx {
class Registry {
public:
  template <typename M, typename... Args>
  metrics::Family<M> &Register(metrics::Description desc, Args &&...args) {
    auto family = std::make_unique<metrics::Family<M>>(
        std::move(desc), std::forward<Args>(args)...);
    auto &ref = *family;
    families_.push_back(std::move(family));
    return ref;
  }

private:
  std::vector<std::unique_ptr<metrics::FamilyBase>> families_;
};
} // namespace promxx
