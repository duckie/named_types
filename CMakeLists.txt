cmake_minimum_required(VERSION 2.8)

project(CppAnonymousStructures)

include_directories(
  ${PROJECT_SOURCE_DIR}/includes/
  ${PROJECT_SOURCE_DIR}/src/
  ${PROJECT_SOURCE_DIR}/test/
  ${PROJECT_BINARY_DIR}/includes/
  ${PROJECT_BINARY_DIR}/src/
  ${PROJECT_BINARY_DIR}/test/
)

# C++11
#list(APPEND CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

### Boost support ###
FIND_PACKAGE(Boost REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
  #link_directories(${Boost_LIBRARY_DIRS})
  #  # GCC does not support the boost auto link feature
  #  if(CMAKE_COMPILER_IS_GNUCXX)
  #      set(LIBRARIES "${LIBRARIES}" "${Boost_LIBRARIES}")
  #  endif(CMAKE_COMPILER_IS_GNUCXX)
endif()

#add_subdirectory(src)
add_subdirectory(test)
