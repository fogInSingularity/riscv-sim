add_library(sim STATIC)

target_sources(sim
    PRIVATE
        source/elf_loader.cpp     
        source/decode.cpp
        source/memory.cpp
        source/rv_sim.cpp
        source/dispatch.cpp
)

target_include_directories(sim
    PUBLIC
        include
)

target_link_libraries(sim
    PRIVATE
        sim-flags
        helpers
        isa

        spdlog
        elfio::elfio
        softfloat
)
