# promxx

A modern, header-only C++ library implementing the [Prometheus](https://prometheus.io/) / [OpenMetrics](https://openmetrics.io/) data model. promxx provides thread-safe metric primitives — Counter, Gauge, and Histogram — and is deliberately agnostic about exposition: it exposes raw metric state so you can serialize it to any format (Prometheus text, OpenMetrics, JSON, StatsD, etc.) through whatever transport you choose.

## Features

- **Header-only** — drop `include/` into your project, no compiled dependency required
- **Lock-free** — all state is backed by `std::atomic`; no mutexes, no contention
- **Type-generic** — Counter and Gauge are templates over any arithmetic type
- **OpenMetrics-aligned** — Histogram stores cumulative bucket counts and an `+Inf` bound, matching the wire format directly
- **Zero external dependencies** — only the C++20 standard library

## Requirements

- C++20 compiler (GCC 11+, Clang 13+, MSVC 19.29+)
- CMake 3.23+ (for the CMake integration; not required if you copy headers directly)

## Installation

### Copy headers

Copy `include/promxx/` into your project and add it to your include path. No build step needed.

### CMake (FetchContent)

```cmake
include(FetchContent)
FetchContent_Declare(promxx
    GIT_REPOSITORY https://github.com/your-org/promxx.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(promxx)

target_link_libraries(my_target PRIVATE promxx::promxx)
```

### CMake (install)

```bash
cmake -B build -DPROMXX_INSTALL=ON
cmake --build build
cmake --install build
```

Then in your project:

```cmake
find_package(promxx REQUIRED)
target_link_libraries(my_target PRIVATE promxx::promxx)
```

## Usage

Include the umbrella header to get all metric types:

```cpp
#include <promxx/promxx.h>
```

Or include individual types:

```cpp
#include <promxx/metrics/counter.h>
#include <promxx/metrics/gauge.h>
#include <promxx/metrics/histogram.h>
```

### Counter

A monotonically increasing value. Suitable for request counts, errors, bytes sent.

```cpp
promxx::metrics::Counter<double> requests_total;

requests_total.Inc();          // increment by 1
requests_total.Inc(5.0);       // increment by arbitrary amount

double value = requests_total.Get();
```

### Gauge

A value that can go up and down. Suitable for active connections, queue depth, memory usage.

```cpp
promxx::metrics::Gauge<int64_t> active_connections;

active_connections.Inc();      // +1
active_connections.Dec();      // -1
active_connections.Inc(10);    // +10
active_connections.Set(42);    // set to exact value

int64_t value = active_connections.Get();
```

### Histogram

Tracks the distribution of observations across configurable buckets. Suitable for request latency, payload sizes.

```cpp
// Use the built-in defaults (0.005 → 10.0, 11 buckets)
promxx::metrics::Histogram latency_seconds(promxx::metrics::DefaultBuckets());

latency_seconds.Observe(0.042);
latency_seconds.Observe(1.7);

uint64_t             count   = latency_seconds.GetCount();
double               sum     = latency_seconds.GetSum();
std::vector<uint64_t> counts = latency_seconds.GetBuckets();   // cumulative
std::vector<double>   bounds = latency_seconds.GetBounds();    // includes +Inf
```

#### Bucket helpers

```cpp
// Linearly spaced: 1, 3, 5, 7  (start=1, width=2, count=4)
auto bounds = promxx::metrics::LinearBuckets(1.0, 2.0, 4);

// Exponentially spaced: 1, 2, 4, 8  (start=1, factor=2, count=4)
auto bounds = promxx::metrics::ExponentialBuckets(1.0, 2.0, 4);

// Sensible defaults for latency in seconds
auto bounds = promxx::metrics::DefaultBuckets();
// → {0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1.0, 2.5, 5.0, 10.0}
```

`+Inf` is always appended automatically if not present, so `GetBuckets()` always has one more entry than the bounds you pass in.

## Thread Safety

All metric types are safe to read and write concurrently from any number of threads. Internally they use `std::atomic` with `memory_order_relaxed`, which is sufficient for observability workloads where strict ordering between metric updates is not required.

```cpp
// Safe to call Observe() from multiple threads simultaneously
promxx::metrics::Histogram h(promxx::metrics::DefaultBuckets());

std::vector<std::thread> workers;
for (int i = 0; i < 8; ++i)
    workers.emplace_back([&h] { h.Observe(1.0); });
for (auto& t : workers) t.join();

assert(h.GetCount() == 8);
```

## Building and Testing

```bash
make configure   # cmake -B build
make build       # cmake --build build
make test        # ctest --test-dir build
make format      # clang-format -i ...
make format-check  # dry-run format check (CI)
make rebuild     # clean + configure + build
```

CMake options:

| Option | Default | Description |
|---|---|---|
| `PROMXX_BUILD_TESTS` | ON (top-level) | Build the test suite |
| `PROMXX_INSTALL` | OFF | Generate install targets |
| `PROMXX_WARNINGS_AS_ERRORS` | OFF | Treat compiler warnings as errors |
| `PROMXX_SANITIZERS` | _(empty)_ | Semicolon-separated list: `address;leak;undefined;thread;memory` |

## License

MIT — see [LICENSE](LICENSE).
