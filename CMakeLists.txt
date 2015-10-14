cmake_minimum_required(VERSION 2.8)
project(CppNamedTypes)
enable_testing()

# Referencing local modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/test/cmake)

# MSVC Needs the header file list to display them correctly
set(HEADER_FILES)
if (MSVC)
  file(GLOB_RECURSE HEADER_FILES includes/*.h*)
endif()

add_subdirectory(test)
