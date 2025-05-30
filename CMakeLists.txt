cmake_minimum_required(VERSION 3.10)
project(Test_banking)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

add_library(banking STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/banking/Transaction.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/banking/Account.cpp
)
target_include_directories(banking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/banking)

if(BUILD_TESTS)
    add_compile_options(--coverage)
    
    add_subdirectory(third-party/gtest)
    
    add_executable(tests
        tests/AccountTest.cpp
        tests/TransactionTest.cpp
    )
    
    target_include_directories(tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/banking
    )
    
    target_link_libraries(tests
        banking
        gtest_main
        gmock_main
    )
    
    add_test(NAME banking_tests COMMAND tests)
endif()
