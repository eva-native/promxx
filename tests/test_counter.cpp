#include <gtest/gtest.h>

#include <cstdint>
#include <thread>
#include <vector>

#include <promxx/metrics/counter.h>

using promxx::metrics::Counter;
using promxx::metrics::CounterBase;

TEST(CounterTest, StartsAtZero) {
  Counter c;
  EXPECT_EQ(c.Get(), 0u);
}

TEST(CounterTest, Inc) {
  Counter c;
  c.Inc();
  EXPECT_EQ(c.Get(), 1u);
}

TEST(CounterTest, IncByAmount) {
  Counter c;
  c.Inc(5u);
  EXPECT_EQ(c.Get(), 5u);
}

TEST(CounterTest, MultipleIncs) {
  Counter c;
  c.Inc();
  c.Inc(3u);
  c.Inc();
  EXPECT_EQ(c.Get(), 5u);
}

TEST(CounterTest, FloatCounter) {
  CounterBase<double> c;
  c.Inc(1.5);
  c.Inc(2.5);
  EXPECT_DOUBLE_EQ(c.Get(), 4.0);
}

TEST(CounterTest, ThreadSafety) {
  Counter c;
  constexpr int kThreads = 8;
  constexpr int kIncrements = 10'000;

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int i = 0; i < kThreads; ++i) {
    threads.emplace_back([&c] {
      for (int j = 0; j < kIncrements; ++j)
        c.Inc();
    });
  }
  for (auto &t : threads)
    t.join();

  EXPECT_EQ(c.Get(), static_cast<std::uint64_t>(kThreads * kIncrements));
}
