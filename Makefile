BUILD_DIR        ?= build
BUILD_TYPE       ?= RelWithDebInfo
CMAKE            ?= cmake
CTEST            ?= ctest
JOBS             ?= $(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu)
SANITIZERS       ?= address;undefined

SOURCES := $(shell find include src tests -name '*.h' -o -name '*.cpp' 2>/dev/null)

.PHONY: all configure build test format format-check clean rebuild help

all: build

configure: ## Configure the CMake project
	$(CMAKE) -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		$(if $(SANITIZERS),-DPROMXX_SANITIZERS="$(SANITIZERS)")

build: configure ## Build the project
	$(CMAKE) --build $(BUILD_DIR) --parallel $(JOBS)

test: build ## Run all tests
	$(CTEST) --test-dir $(BUILD_DIR) --output-on-failure

format: ## Format all sources in-place
	clang-format -i $(SOURCES)

format-check: ## Check formatting without modifying files (CI-friendly)
	clang-format --dry-run --Werror $(SOURCES)

clean: ## Remove the build directory
	$(CMAKE) --build $(BUILD_DIR) --target clean 2>/dev/null || true

rebuild: ## Clean and rebuild from scratch
	rm -rf $(BUILD_DIR)
	$(MAKE) build

help: ## Show this help
	@grep -E '^[a-zA-Z_-]+:.*##' $(MAKEFILE_LIST) \
		| awk 'BEGIN {FS = ":.*##"}; {printf "  %-16s %s\n", $$1, $$2}'
