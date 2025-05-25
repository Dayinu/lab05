cmake_minimum_required(VERSION 3.10)
project(BankingProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(banking
    banking/Account.cpp
    banking/Transaction.cpp
)

target_include_directories(banking PUBLIC
    /banking
)

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
    enable_testing()
    
    find_package(GTest REQUIRED)
    
    add_executable(banking_tests
        tests/AccountTest.cpp
        tests/TransactionTest.cpp
    )
    
    target_link_libraries(banking_tests
        PRIVATE
        banking
        GTest::GTest
        GTest::Main
    )
    
    add_test(NAME banking_tests COMMAND banking_tests)
endif()
