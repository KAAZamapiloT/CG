cmake_minimum_required(VERSION 3.20)
project(PhongDemo C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    # Architecture hint for the bundled Windows SDL3 package
    if(NOT DEFINED SDL_CPU_X64 AND NOT DEFINED SDL_CPU_X86 AND NOT DEFINED SDL_CPU_ARM64)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(SDL_CPU_X64 1)
        else()
            set(SDL_CPU_X86 1)
        endif()
    endif()

    list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/SDL3-3.4.0")
    find_package(SDL3 REQUIRED CONFIG)
else()
    # 1. Check if SDL3 source is present locally in a folder
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL3-linux/CMakeLists.txt")
        add_subdirectory(SDL3-linux)
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SDL3/CMakeLists.txt")
        add_subdirectory(SDL3)
    else()
        # 2. Look for system SDL3
        find_package(SDL3 CONFIG QUIET)
        if(NOT SDL3_FOUND)
            find_package(PkgConfig QUIET)
            if(PkgConfig_FOUND)
                pkg_check_modules(SDL3 QUIET IMPORTED_TARGET sdl3)
                if(TARGET PkgConfig::SDL3)
                    add_library(SDL3::SDL3 ALIAS PkgConfig::SDL3)
                    set(SDL3_FOUND TRUE)
                endif()
            endif()
        endif()

        # 3. Fallback to FetchContent if not found
        if(NOT SDL3_FOUND AND NOT TARGET SDL3::SDL3)
            message(STATUS "SDL3 not found on system or local folder. Fetching SDL3 via FetchContent...")
            include(FetchContent)
            FetchContent_Declare(
                SDL3
                GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
                GIT_TAG release-3.2.8
                GIT_SHALLOW TRUE
            )
            FetchContent_MakeAvailable(SDL3)
        endif()
    endif()
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLAD OpenGL loader library
add_library(glad STATIC
    glad/src/gl.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include
)

# Main executable
add_executable(phong
    phong.cpp
)

target_link_libraries(phong PRIVATE
    glad
    SDL3::SDL3
    OpenGL::GL
)

# On Windows: copy SDL3.dll next to the executable
if(WIN32 AND TARGET SDL3::SDL3-shared)
    add_custom_command(TARGET phong POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE_DIR:phong>
        COMMENT "Copying SDL3.dll to executable directory..."
        VERBATIM
    )
endif()

