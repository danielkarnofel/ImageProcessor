cmake_minimum_required(VERSION 3.16)
project(ImageProcessor LANGUAGES CXX)

# ---- C++ Standard ----
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Include Directories ----
include_directories(
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/inc/stb
)

# ---- Source Files ----
set(SOURCES
    src/main.cpp
    src/stb/stb_image.cpp
    src/stb/stb_image_write.cpp
)

# ---- Executable ----
add_executable(ImageProcessor ${SOURCES})

# ---- Output Directory ----
set_target_properties(ImageProcessor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

target_compile_definitions(ImageProcessor PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# ---- Platform-Specific Warnings / Flags ----
if (MSVC)
    target_compile_options(ImageProcessor PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(ImageProcessor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---- Optional: Copy input/output folders on build ----
add_custom_command(TARGET ImageProcessor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_SOURCE_DIR}/output
)
