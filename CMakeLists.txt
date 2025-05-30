cmake_minimum_required(VERSION 3.4)
project(Test_banking)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
  add_compile_options(--coverage)
endif()

add_library(banking STATIC ${CMAKE_CURRENT_SOURCE_DIR}/banking/Transaction.cpp ${CMAKE_CURRENT_SOURCE_DIR}/banking/Account.cpp)
target_include_directories(banking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/banking)
target_link_libraries(banking gcov)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)
  file(GLOB BANKING_TEST_SOURCES tests/*.cpp)
  add_executable(check ${BANKING_TEST_SOURCES})
  target_link_libraries(check banking gtest_main gmock_main)
  add_test(NAME check COMMAND check)
endif()

find_package(GTest REQUIRED COMPONENTS gmock_main)

add_executable(tests
    tests/AccountTest.cpp
    tests/TransactionTest.cpp
)

target_link_libraries(tests
    PRIVATE
    banking
    GTest::gmock_main
)
