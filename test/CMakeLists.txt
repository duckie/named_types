include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
Find_Package(Threads REQUIRED)
Find_Package(GTest)
Find_Package(rapidjson)

include_directories(
  ${PROJECT_SOURCE_DIR}/includes/
  ${PROJECT_SOURCE_DIR}/test/
  ${PROJECT_BINARY_DIR}/includes/
  ${PROJECT_BINARY_DIR}/test/
  ${GTEST_INCLUDE_DIRS}
)

# Compilation test for string literal operator template
check_cxx_source_compiles("
template <class T, T ... chars> struct string_literal {};
template <typename T, T... chars>  constexpr string_literal<T,chars...> operator \"\"_t () { return {}; }
int main(){return 0;}" compiles_string_literal_operator_template)
if (${compiles_string_literal_operator_template})
  message("-- Support for string literal operator template FOUND.")
else()
  unset(compiles_string_literal_operator_template CACHE)
  message(WARNING 
    "Support for string literal operator template NOT FOUND\n"
    "Some are features disabled.\n"
    "MSVC is known not to support it.\n")
endif ()

add_custom_target(build-test)

# "nt" stands for "named tuple"
if (${GTEST_FOUND})
  add_library(test-main OBJECT test-main.cc)
endif()

function(add_nt_test name)
  if("${ARGV1}" STREQUAL "GT")
    if (GTEST_FOUND)
      add_executable(${name} ${name}.cc $<TARGET_OBJECTS:test-main> ${HEADER_FILES})
      target_link_libraries(${name} ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
      project_add_test(${name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name})  
    endif()
  else()
    add_executable(${name} ${name}.cc ${HEADER_FILES})
    target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT})
    project_add_test(${name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name})  
  endif()
endfunction()

#add_nt_test(unit_tests GT)
add_nt_test(named_tuple_tests GT)
add_nt_test(named_tuple_extensions_tests GT)
add_nt_test(named_tuple_generation_tests GT)
add_nt_test(demo2)
add_nt_test(demo3)
add_nt_test(demo5)
add_nt_test(demo6)

# Tests with string literal operator
if (${compiles_string_literal_operator_template})
  add_nt_test(named_tuple_tests_cpp1z GT)
  add_nt_test(demo1)
  add_nt_test(demo4)
endif()

# Rapidjson extension
if (${RAPIDJSON_FOUND})
  include_directories(${RAPIDJSON_INCLUDE_DIRS})
  add_nt_test(named_tuple_rapidjson_tests GT)
endif()

# Compile examples
add_subdirectory(examples)
