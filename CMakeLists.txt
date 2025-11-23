cmake_minimum_required(VERSION 3.21)

project(riscv-sim)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

add_subdirectory(flags)
add_subdirectory(third_party)

add_subdirectory(src/helpers)
add_subdirectory(src/sim)
add_subdirectory(src/isa)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
        src/main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        include/
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        sim-flags
        helpers
        sim

        spdlog
)

# enable_testing()
# add_subdirectory(tests)
