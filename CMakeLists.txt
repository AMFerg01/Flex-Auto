cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")




project(flexauto)

find_package(Python3 REQUIRED Interpreter Development)

set(PYTHON_LIBRARY_DIR ${Python3_SITEARCH})
set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})

include_directories("${CMAKE_SOURCE_DIR}/cpp/include/flex_include")
include_directories("${CMAKE_SOURCE_DIR}/py")

file (GLOB SOURCE_FILES "cpp/src/*.cpp")
file (GLOB HEADER_FILES "cpp/include/flex_include/*.hpp")
file (GLOB PYTHON_FILES "py/*.cpp" "py/*.hpp")

# Set up such that XCode organizes the files
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES} ${HEADER_FILES} ${PYTHON_FILES} )
set(pybind11_DIR ~/.local/lib/python3.10/site-packages/pybind11/share/cmake/pybind11)

find_package(pybind11 REQUIRED)
pybind11_add_module(flexauto 
	${SOURCE_FILES}
	${HEADER_FILES}
	${PYTHON_FILES}
)

target_link_libraries(flexauto PUBLIC)

install(TARGETS flexauto
  COMPONENT python
  LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}"
  )