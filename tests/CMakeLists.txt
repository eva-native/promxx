CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    VERSION 1.15.2
    OPTIONS
        "INSTALL_GTEST OFF"
        # "gtest_force_shared_crt ON"
)

add_executable(promxx_tests
    test_counter.cpp
    test_gauge.cpp
    test_histogram.cpp
)

target_link_libraries(promxx_tests
    PRIVATE
        promxx::promxx
        GTest::gtest_main
)

target_compile_features(promxx_tests PRIVATE cxx_std_20)

set_target_properties(promxx_tests PROPERTIES CXX_EXTENSIONS OFF)

include(GoogleTest)
gtest_discover_tests(promxx_tests)
