include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.15.0
    GIT_SHALLOW true
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
    ELFIO
    GIT_REPOSITORY https://github.com/serge1/ELFIO
    GIT_TAG Release_3.12
    GIT_SHALLOW true
)
FetchContent_MakeAvailable(ELFIO)


