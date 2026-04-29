cmake_minimum_required(VERSION 3.23)

project(promxx VERSION 0.0.0 DESCRIPTION "Prometheus client" LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Use a separate build directory.")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

option(PROMXX_INSTALL "Build for install" OFF)
option(PROMXX_BUILD_TESTS "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(PROMXX_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

set(PROMXX_SANITIZERS "" CACHE STRING "Semicolon-separated list of sanitizers: address;leak;undefined;thread;memory")

include(cmake/CPM.cmake)
include(cmake/Warnings.cmake)
include(cmake/Sanitizers.cmake)

add_library(promxx-warnings INTERFACE)
promxx_set_compiler_warnings(TARGET promxx-warnings TREAT_AS_ERROR ${PROMXX_WARNINGS_AS_ERRORS})

add_library(promxx-sanitizers INTERFACE)
promxx_enable_sanitizers(TARGET promxx-sanitizers SANITIZERS ${PROMXX_SANITIZERS})

add_library(promxx)
add_library(promxx::promxx ALIAS promxx)

target_sources(promxx
    PRIVATE
        src/promxx.cpp
        src/label.cpp
        src/metrics/family_base.cpp
    PUBLIC FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/promxx/promxx.h
            include/promxx/registry.h

            include/promxx/metrics/description.h
            include/promxx/metrics/gauge.h
            include/promxx/metrics/counter.h
            include/promxx/metrics/histogram.h
            include/promxx/metrics/family.h
            include/promxx/metrics/family_base.h

            include/promxx/meta/concepts.h

            include/promxx/detail/noncopybale.h
)

if(${PROMXX_INSTALL})
  target_link_libraries(promxx)
else()
  target_link_libraries(promxx PRIVATE promxx-warnings promxx-sanitizers)
endif()

target_compile_features(promxx PUBLIC cxx_std_20)

set_target_properties(promxx PROPERTIES
    VERSION       ${PROJECT_VERSION}
    SOVERSION     ${PROJECT_VERSION_MAJOR}
    CXX_EXTENSIONS OFF
)

if(${PROMXX_INSTALL})
  include(GNUInstallDirs)

  install(TARGETS promxx
      EXPORT      promxxTargets
      LIBRARY     DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE     DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME     DESTINATION ${CMAKE_INSTALL_BINDIR}
      FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  install(EXPORT promxxTargets
      FILE        promxxTargets.cmake
      NAMESPACE   promxx::
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/promxx
  )

  include(CMakePackageConfigHelpers)

  write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/promxxConfigVersion.cmake"
      VERSION          ${PROJECT_VERSION}
      COMPATIBILITY    SameMajorVersion
  )

  configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/promxxConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/promxxConfig.cmake"
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/promxx
  )

  install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/promxxConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/promxxConfigVersion.cmake"
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/promxx
  )
endif()

if(PROMXX_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
