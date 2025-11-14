cmake_minimum_required(VERSION 3.15)

# Example programs
set(EXAMPLE_SOURCES
    black_scholes_example.cpp
    american_option_example.cpp
    asian_option_example.cpp
    barrier_option_example.cpp
    heston_example.cpp
    volatility_surface_example.cpp
)

# Create example executables
foreach(example_source ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_source} NAME_WE)
    add_executable(${example_name} ${example_source})
    target_link_libraries(${example_name} PRIVATE derivatives_pricing)
endforeach()
