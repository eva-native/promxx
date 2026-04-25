#pragma once

#include <string>

namespace promxx::metrics {
struct Description {
  std::string name;
  std::string help;
  std::string unit;
};
}
