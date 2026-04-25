#include "promxx/metrics/family_base.h"

#include <utility>

namespace promxx::metrics {
FamilyBase::FamilyBase(Description desc) noexcept : desc_(std::move(desc)) {}
} // namespace promxx::metrics
