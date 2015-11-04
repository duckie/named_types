function(add_example name)
  add_executable(${name} ${name}.cc ${HEADER_FILES})
  target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT})
  add_dependencies(build-test ${name})
  add_test(${name} ${name})  
endfunction()

add_example(factory)
