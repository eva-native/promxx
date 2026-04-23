#include <gtest/gtest.h>

#include <limits>
#include <thread>

#include <promxx/metrics/histogram.h>

using promxx::metrics::DefaultBuckets;
using promxx::metrics::ExponentialBuckets;
using promxx::metrics::Histogram;
using promxx::metrics::LinearBuckets;

TEST(HistogramTest, CountAndSum) {
  Histogram h({1.0, 5.0, 10.0});
  h.Observe(2.0);
  h.Observe(4.0);
  h.Observe(8.0);
  EXPECT_EQ(h.GetCount(), 3u);
  EXPECT_DOUBLE_EQ(h.GetSum(), 14.0);
}

TEST(HistogramTest, BucketAssignment) {
  Histogram h({1.0, 5.0, 10.0});
  h.Observe(0.5);  // bucket [le=1]
  h.Observe(1.0);  // bucket [le=1]  (equal to bound)
  h.Observe(3.0);  // bucket [le=5]
  h.Observe(20.0); // bucket [le=+Inf]

  auto buckets = h.GetBuckets();
  ASSERT_EQ(buckets.size(), 4u); // 1.0, 5.0, 10.0, +Inf
  EXPECT_EQ(buckets[0], 2u);     // le=1.0  : 0.5, 1.0
  EXPECT_EQ(buckets[1], 3u);     // le=5.0  : cumulative
  EXPECT_EQ(buckets[2], 3u);     // le=10.0 : cumulative
  EXPECT_EQ(buckets[3], 4u);     // le=+Inf : all
}

TEST(HistogramTest, CumulativeBuckets) {
  Histogram h({1.0, 2.0, 3.0});
  h.Observe(0.5);
  h.Observe(1.5);
  h.Observe(2.5);

  auto buckets = h.GetBuckets();
  EXPECT_EQ(buckets[0], 1u);
  EXPECT_EQ(buckets[1], 2u);
  EXPECT_EQ(buckets[2], 3u);
  EXPECT_EQ(buckets[3], 3u); // +Inf auto-appended
}

TEST(HistogramTest, InfAutoAppended) {
  Histogram h({1.0, 5.0});
  const auto &bounds = h.GetBounds();
  ASSERT_EQ(bounds.size(), 3u);
  EXPECT_EQ(bounds.back(), std::numeric_limits<double>::infinity());
}

TEST(HistogramTest, ExplicitInfNotDuplicated) {
  Histogram h({1.0, std::numeric_limits<double>::infinity()});
  EXPECT_EQ(h.GetBounds().size(), 2u);
}

TEST(HistogramTest, InfBucket) {
  Histogram h({1.0});
  h.Observe(1e9);
  auto buckets = h.GetBuckets();
  EXPECT_EQ(buckets[0], 0u); // le=1.0
  EXPECT_EQ(buckets[1], 1u); // le=+Inf
}

TEST(HistogramTest, EmptyBoundsThrows) {
  EXPECT_THROW(Histogram<> h({}), std::invalid_argument);
}

TEST(HistogramTest, DefaultBuckets) {
  Histogram h(DefaultBuckets());
  EXPECT_EQ(h.GetBounds().size(), 12u); // 11 buckets + +Inf
  h.Observe(0.3);
  EXPECT_EQ(h.GetCount(), 1u);
}

TEST(HistogramTest, LinearBuckets) {
  auto bounds = LinearBuckets(1.0, 2.0, 4);
  ASSERT_EQ(bounds.size(), 4u);
  EXPECT_DOUBLE_EQ(bounds[0], 1.0);
  EXPECT_DOUBLE_EQ(bounds[1], 3.0);
  EXPECT_DOUBLE_EQ(bounds[2], 5.0);
  EXPECT_DOUBLE_EQ(bounds[3], 7.0);
}

TEST(HistogramTest, ExponentialBuckets) {
  auto bounds = ExponentialBuckets(1.0, 2.0, 4);
  ASSERT_EQ(bounds.size(), 4u);
  EXPECT_DOUBLE_EQ(bounds[0], 1.0);
  EXPECT_DOUBLE_EQ(bounds[1], 2.0);
  EXPECT_DOUBLE_EQ(bounds[2], 4.0);
  EXPECT_DOUBLE_EQ(bounds[3], 8.0);
}

TEST(HistogramTest, LinearBucketsInvalidCount) {
  EXPECT_THROW(LinearBuckets(1.0, 1.0, 0), std::invalid_argument);
}

TEST(HistogramTest, ExponentialBucketsInvalidArgs) {
  EXPECT_THROW(ExponentialBuckets(0.0, 2.0, 4), std::invalid_argument);
  EXPECT_THROW(ExponentialBuckets(1.0, 1.0, 4), std::invalid_argument);
  EXPECT_THROW(ExponentialBuckets(1.0, 2.0, 0), std::invalid_argument);
}

TEST(HistogramTest, ThreadSafety) {
  Histogram h({1.0, 10.0, 100.0});
  constexpr int kThreads = 8;
  constexpr int kObservations = 10'000;

  std::vector<std::thread> threads;
  threads.reserve(kThreads);
  for (int i = 0; i < kThreads; ++i) {
    threads.emplace_back([&h] {
      for (int j = 0; j < kObservations; ++j)
        h.Observe(5.0);
    });
  }
  for (auto &t : threads)
    t.join();

  EXPECT_EQ(h.GetCount(), static_cast<std::uint64_t>(kThreads * kObservations));
  EXPECT_DOUBLE_EQ(h.GetSum(), 5.0 * kThreads * kObservations);
  EXPECT_EQ(h.GetBuckets().back(),
            static_cast<std::uint64_t>(kThreads * kObservations));
}
