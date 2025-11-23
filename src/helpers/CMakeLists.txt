add_library(helpers INTERFACE)

# target_sources(helpers
#     PRIVATE
# )

target_include_directories(helpers
    INTERFACE
        include
)

target_link_libraries(helpers
    INTERFACE
        sim-flags
        spdlog
        dl
)
