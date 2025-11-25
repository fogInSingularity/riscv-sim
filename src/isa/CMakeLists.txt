add_library(isa STATIC)

target_sources(isa
    PRIVATE
        source/ext_i_cb.cpp 
        source/mnemonics.cpp
        source/syscall_handler.cpp
)

target_include_directories(isa
    PUBLIC
        include
)

target_link_libraries(isa
    PRIVATE
        sim-flags
        sim
        helpers
)
