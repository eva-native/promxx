#pragma once

#include "promxx/detail/noncopybale.h"
#include "promxx/metrics/description.h"

namespace promxx::metrics {
class FamilyBase : ::promxx::detail::Noncopybale {
public:
  explicit FamilyBase(Description desc) noexcept;
private:
  Description desc_;
};
}
