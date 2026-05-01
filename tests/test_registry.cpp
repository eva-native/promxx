#include <gtest/gtest.h>

#include <promxx/metrics/counter.h>
#include <promxx/registry.h>

using promxx::Registry;
using promxx::metrics::Counter;

TEST(RegistryTest, Sample) {
  auto registry = Registry();
  auto &counter_fam = registry.Register<Counter>(promxx::metrics::Description{"counter", "counter family"});
  auto &get_counter = counter_fam.Add({{"method", "GET"}});
  EXPECT_EQ(get_counter.Get(), 0u);
  get_counter.Inc();
  EXPECT_EQ(get_counter.Get(), 1u);
}
