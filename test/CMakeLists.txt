Find_Package(Threads REQUIRED)
Find_Package(GTest REQUIRED)

include_directories(
  ${PROJECT_SOURCE_DIR}/includes/
  ${PROJECT_SOURCE_DIR}/src/
  ${PROJECT_SOURCE_DIR}/test/
  ${PROJECT_BINARY_DIR}/includes/
  ${PROJECT_BINARY_DIR}/src/
  ${PROJECT_BINARY_DIR}/test/
  ${GTEST_INCLUDE_DIRS}
)

# C++11 is a requirement here
include(CheckCXXCompilerFlag)
#check_cxx_compiler_flag(-std=c++1y HAVE_STD_CPP11_FLAG)
#if (HAVE_STD_CPP11_FLAG)
#  message("-- C++14 support found.")
#  add_definitions(-D NAMED_TUPLES_CPP14)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
#else()
  check_cxx_compiler_flag(-std=c++11 HAVE_STD_CPP11_FLAG)
  if (HAVE_STD_CPP11_FLAG)
    message("-- C++11 support found.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    message(FATAL_ERROR "Your compiler must support at least C++11 to compile ${PROJECT_NAME}.")
  endif()
#endif()

add_library(test-main OBJECT test-main.cc)
add_custom_target(build-test)

# "nt" stands for "named tuple"
function(add_nt_test name)
  if("${ARGV1}" STREQUAL "GT")
    add_executable(${name} ${name}.cc $<TARGET_OBJECTS:test-main>)
  else()
    add_executable(${name} ${name}.cc)
  endif()
  target_link_libraries(${name} ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  add_dependencies(build-test ${name})
  add_test(${name} ${name})  
endfunction()

add_nt_test(unit_tests GT)
add_nt_test(demo1)
add_nt_test(demo2)
add_nt_test(demo3)
add_nt_test(demo4)

