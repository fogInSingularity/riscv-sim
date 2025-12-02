include(FetchContent)
include(ExternalProject)

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

# softfloat
# Magic so CMake build and link library properly
ExternalProject_Add(
    softfloat_ext
        GIT_REPOSITORY  https://github.com/ucb-bar/berkeley-softfloat-3.git
        GIT_TAG         a0c6494

        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE ON

        BUILD_COMMAND    ${CMAKE_MAKE_PROGRAM} -C <SOURCE_DIR>/build/Linux-x86_64-GCC -j4

        INSTALL_COMMAND  ""
        UPDATE_COMMAND   ""
)
ExternalProject_Get_Property(softfloat_ext SOURCE_DIR)

add_library(softfloat_lib STATIC IMPORTED GLOBAL)
set_target_properties(softfloat_lib
    PROPERTIES
        IMPORTED_LOCATION "${SOURCE_DIR}/build/Linux-x86_64-GCC/softfloat.a"
)
add_dependencies(softfloat_lib softfloat_ext)

add_library(softfloat_headers INTERFACE)
target_include_directories(softfloat_headers 
    INTERFACE
        .
        "${SOURCE_DIR}/source/include"
)

add_library(softfloat INTERFACE)
target_link_libraries(softfloat 
    INTERFACE 
        softfloat_lib
        softfloat_headers
)

