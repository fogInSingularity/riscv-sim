cmake_minimum_required(VERSION 3.21)

project(riscv-sim)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

add_subdirectory(flags)
add_subdirectory(third_party)

add_subdirectory(src/helpers)
add_subdirectory(src/sim)
add_subdirectory(src/isa)

add_executable(riscv-sim.x)

target_sources(riscv-sim.x
    PRIVATE
        src/main.cpp
)

target_include_directories(riscv-sim.x
    PRIVATE
        include/
)

target_link_libraries(riscv-sim.x
    PRIVATE 
        sim-flags
        helpers
        sim
        isa

        spdlog
        softfloat
)

# enable_testing()
# add_subdirectory(tests)
