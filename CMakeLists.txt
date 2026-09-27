cmake_minimum_required(VERSION 3.20)

project(AI_Learn_to_drive
    VERSION 1.0
    LANGUAGES CXX
)

# ---------------------------------------------------------
# C++ standard
# ---------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------
# Raylib
# ---------------------------------------------------------

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------
# Project sources
# ---------------------------------------------------------

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
)

add_executable(AI_Learn_to_drive
    Main.cpp
    ${PROJECT_SOURCES}
)

# ---------------------------------------------------------
# Include directories
# ---------------------------------------------------------

target_include_directories(AI_Learn_to_drive PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/include"
)

# ---------------------------------------------------------
# Libraries
# ---------------------------------------------------------

target_link_libraries(AI_Learn_to_drive PRIVATE
    raylib
)

if (WIN32)
    target_compile_definitions(AI_Learn_to_drive PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
endif()

if (MSVC)
    target_compile_definitions(AI_Learn_to_drive PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
endif()