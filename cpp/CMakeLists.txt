cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(flexauto VERSION 0.0.0)

# Include dir
include_directories(/usr/local/include)

# Src
AUX_SOURCE_DIRECTORY(src SRC_FILES)

# Headers
set(PROJECT_SOURCE_DIR "src")
set(PROJECT_INCLUDE_DIR "include/flex_include")

# Source files
set(SOURCE_FILES
    ${PROJECT_INCLUDE_DIR}/Autocallable.hpp
    ${PROJECT_SOURCE_DIR}/Autocallable.cpp
    ${PROJECT_INCLUDE_DIR}/BrownianModel.hpp
    ${PROJECT_SOURCE_DIR}/BrownianModel.cpp
    ${PROJECT_INCLUDE_DIR}/debug_utilities.hpp

    )

# Set up such that XCode organizes the files correctly
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Add library
add_library(flexauto SHARED ${SOURCE_FILES})

# Include directories
target_include_directories(flexauto PRIVATE include/flex_include)

# Install
install(TARGETS flexauto DESTINATION lib)

# Install the headers
install(FILES include/flex DESTINATION include)

# Create base directory
install(DIRECTORY include/flex_include DESTINATION include)