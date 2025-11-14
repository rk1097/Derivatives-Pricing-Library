cmake_minimum_required(VERSION 3.15)
project(DerivativesPricingLibrary VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(MATH_SOURCES
    src/math/statistics.cpp
    src/math/matrix.cpp
    src/math/interpolation.cpp
)

set(MODEL_SOURCES
    src/models/black_scholes.cpp
    src/models/binomial_tree.cpp
    src/models/monte_carlo.cpp
    src/models/lsmc.cpp
    src/models/heston.cpp
)

set(CURVE_SOURCES
    src/curves/volatility_surface.cpp
)

set(LIBRARY_SOURCES
    ${MATH_SOURCES}
    ${MODEL_SOURCES}
    ${CURVE_SOURCES}
)

# Create library
add_library(derivatives_pricing ${LIBRARY_SOURCES})

target_include_directories(derivatives_pricing
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Set library properties
set_target_properties(derivatives_pricing PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/derivatives
)

# Link math library
target_link_libraries(derivatives_pricing PUBLIC m)

# Install targets
install(TARGETS derivatives_pricing
    EXPORT DerivativesPricingTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

install(DIRECTORY include/derivatives
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(EXPORT DerivativesPricingTargets
    FILE DerivativesPricingTargets.cmake
    NAMESPACE DerivativesPricing::
    DESTINATION lib/cmake/DerivativesPricing
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DerivativesPricingConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(cmake/DerivativesPricingConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/DerivativesPricingConfig.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DerivativesPricingConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DerivativesPricingConfigVersion.cmake"
    DESTINATION lib/cmake/DerivativesPricing
)

# Build tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Build examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
