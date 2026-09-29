# Builds every project in this folder with CMake, on macOS, Linux, or
# Windows. Appendix B of the book, For Mac and Linux Users, explains it.
#
# Every folder here with .cpp files in it becomes a program called game,
# built into that folder, beside its assets, so it runs as it does in
# Visual Studio. A folder of your own works the same way: make it, put
# your .cpp and .h files in it, and configure again.

cmake_minimum_required(VERSION 3.24)
project(SDL3Projects LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# On Windows, use the copies of SDL that sit beside the projects
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL3"
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL3_image"
        "${CMAKE_CURRENT_SOURCE_DIR}/SDL3_ttf")
endif()

# Only what the book's games need: no SDL tests, no image formats that
# need other libraries (PNG needs none), and none of SDL_ttf's extras
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_PNG_LIBPNG OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)
set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)

# SDL, SDL_image, and SDL_ttf: if they're installed already, in new enough
# versions, CMake uses them, and if not, it downloads and builds them, the
# first time only
include(FetchContent)
FetchContent_Declare(SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.4/SDL3-3.4.4.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS 3.4 CONFIG)
FetchContent_Declare(SDL3_image
    URL https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.2/SDL3_image-3.4.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS 3.4 CONFIG)
FetchContent_Declare(SDL3_ttf
    URL https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    FIND_PACKAGE_ARGS 3.2 CONFIG)
FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_ttf)

# One program for each folder that has .cpp files in it
file(GLOB folders LIST_DIRECTORIES true RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(folder IN LISTS folders)
    set(dir "${CMAKE_CURRENT_SOURCE_DIR}/${folder}")
    if(NOT IS_DIRECTORY "${dir}")
        continue()
    endif()
    file(GLOB sources CONFIGURE_DEPENDS "${dir}/*.cpp")
    if(NOT sources)
        continue()
    endif()
    file(GLOB headers CONFIGURE_DEPENDS "${dir}/*.h")

    # A target's name can't have spaces, so "Rogue SDL Part 1" is Rogue_SDL_Part_1
    string(REGEX REPLACE "[^A-Za-z0-9_.+-]" "_" target "${folder}")
    add_executable(${target} ${sources} ${headers})
    target_link_libraries(${target} PRIVATE
        SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image SDL3::SDL3)
    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME game
        RUNTIME_OUTPUT_DIRECTORY "${dir}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${dir}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${dir}")
endforeach()
