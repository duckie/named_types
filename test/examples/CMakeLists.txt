function(add_example name)
  add_executable(${name} ${name}.cc ${HEADER_FILES})
  target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT})
  project_add_test(${name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name})  
endfunction()

add_example(factory)
