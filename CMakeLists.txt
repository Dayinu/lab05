cmake_minimum_required(VERSION 3.10) 
project(Test_banking)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/banking)

if(BUILD_TESTS)
  add_compile_options(--coverage)
  
  add_subdirectory(third-party/gtest)
  
  file(GLOB BANKING_TEST_SOURCES tests/*.cpp)
  add_executable(tests ${BANKING_TEST_SOURCES})
  
  target_link_libraries(tests
    banking
    gtest_main
    gmock_main
  )
  
  add_test(NAME banking_tests COMMAND tests)
endif()
