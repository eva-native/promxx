#include <gtest/gtest.h>

#include <thread>
#include <vector>

#include <promxx/metrics/gauge.h>

using promxx::metrics::Gauge;

TEST(GaugeTest, StartsAtZero) {
  Gauge<> g;
  EXPECT_DOUBLE_EQ(g.Get(), 0.0);
}

TEST(GaugeTest, Inc) {
  Gauge<> g;
  g.Inc();
  EXPECT_DOUBLE_EQ(g.Get(), 1.0);
}

TEST(GaugeTest, IncByAmount) {
  Gauge<> g;
  g.Inc(3.5);
  EXPECT_DOUBLE_EQ(g.Get(), 3.5);
}

TEST(GaugeTest, Dec) {
  Gauge<> g;
  g.Inc(5.0);
  g.Dec();
  EXPECT_DOUBLE_EQ(g.Get(), 4.0);
}

TEST(GaugeTest, DecBelowZero) {
  Gauge<> g;
  g.Dec();
  EXPECT_DOUBLE_EQ(g.Get(), -1.0);
}

TEST(GaugeTest, Set) {
  Gauge<> g;
  g.Inc(100.0);
  g.Set(42.0);
  EXPECT_DOUBLE_EQ(g.Get(), 42.0);
}

TEST(GaugeTest, IntGauge) {
  Gauge<int> g;
  g.Inc(3);
  g.Dec(1);
  EXPECT_EQ(g.Get(), 2);
}

TEST(GaugeTest, ThreadSafety) {
  Gauge<> g;
  constexpr int kThreads = 8;
  constexpr int kOps = 10'000;

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int i = 0; i < kThreads; ++i) {
    threads.emplace_back([&g] {
      for (int j = 0; j < kOps; ++j) {
        g.Inc(1.0);
        g.Dec(1.0);
      }
    });
  }
  for (auto &t : threads)
    t.join();

  EXPECT_DOUBLE_EQ(g.Get(), 0.0);
}
