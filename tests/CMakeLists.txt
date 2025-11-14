cmake_minimum_required(VERSION 3.15)

# Test files
set(TEST_SOURCES
    test_black_scholes.cpp
    test_binomial_tree.cpp
    test_monte_carlo.cpp
    test_options.cpp
)

# Create test executables
foreach(test_source ${TEST_SOURCES})
    get_filename_component(test_name ${test_source} NAME_WE)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} PRIVATE derivatives_pricing)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
