## Домашние задание 5

**Студентки группы ИУ8-22**

**Ивановой Влады**

### Задание
1. Создайте `CMakeList.txt` для библиотеки *banking*.
2. Создайте модульные тесты на классы `Transaction` и `Account`.
    * Используйте mock-объекты.
    * Покрытие кода должно составлять 100%.
3. Настройте сборочную процедуру на **TravisCI**.
4. Настройте [Coveralls.io](https://coveralls.io/).
```sh
$ cd lab05        
                                                                                     
$ cat << EOF > CMakeLists.txt
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
EOF
                                                                                     
$ mkdir -p tests   
                                                                                     
$ cd tests                   
                                                                                     
$ cat << EOF > AccountTest.cpp
#include <Account.h>
#include <gtest/gtest.h>

TEST(Account, Banking){
	Account test(0,0);
	
	ASSERT_EQ(test.GetBalance(), 0);
	
	ASSERT_THROW(test.ChangeBalance(100), std::runtime_error);
	
	test.Lock();
	
	ASSERT_NO_THROW(test.ChangeBalance(100));
	
	ASSERT_EQ(test.GetBalance(), 100);

	ASSERT_THROW(test.Lock(), std::runtime_error);

	test.Unlock();
	ASSERT_THROW(test.ChangeBalance(100), std::runtime_error);
}
EOF

                                                                                     
$ cat << EOF > TransactionTest.cpp
#include <Account.h>
#include <Transaction.h>
#include <gtest/gtest.h>

TEST(Transaction, Banking) {
    const int initial_balance_Alice = 10000;
    const int initial_balance_Bob = 2000;
    const int transaction_fee = 50;

    Account Alice(0, initial_balance_Alice), Bob(1, initial_balance_Bob);
    Transaction test_tran;

    ASSERT_EQ(test_tran.fee(), 1);
    test_tran.set_fee(transaction_fee);
    ASSERT_EQ(test_tran.fee(), transaction_fee);

    ASSERT_THROW(test_tran.Make(Alice, Alice, 2000), std::logic_error);
    ASSERT_THROW(test_tran.Make(Alice, Bob, -100), std::invalid_argument);
    ASSERT_THROW(test_tran.Make(Alice, Bob, 50), std::logic_error);

    if (test_tran.fee() * 2 - 1 >= 200) {
        ASSERT_FALSE(test_tran.Make(Alice, Bob, test_tran.fee() * 2 - 1));
    }

    Alice.Lock();
    ASSERT_THROW(test_tran.Make(Alice, Bob, 2000), std::runtime_error);
    Alice.Unlock();

    ASSERT_TRUE(test_tran.Make(Alice, Bob, 2000));
    ASSERT_EQ(Bob.GetBalance(), initial_balance_Bob + 2000);	
    ASSERT_EQ(Alice.GetBalance(), initial_balance_Alice - 2000 - transaction_fee);

    ASSERT_FALSE(test_tran.Make(Alice, Bob, 8000));
    ASSERT_EQ(Bob.GetBalance(), initial_balance_Bob + 2000);	
    ASSERT_EQ(Alice.GetBalance(), initial_balance_Alice - 2000 - transaction_fee);
}
EOF
                                                                                     
$ mkdir -p .github/workflows
                                                                                     
$ cat << EOF > .github/workflows/ci.yml
name: Actions_for_tests

on:
 push:
  branches: [main]
 pull_request:
  branches: [main]

jobs: 
 build_Linux:

  runs-on: ubuntu-latest

  steps:
  - uses: actions/checkout@v3

  - name: Adding gtest
    run: git clone https://github.com/google/googletest.git third-party/gtest -b release-1.11.0

  - name: Install lcov
    run: sudo apt-get install -y lcov

  - name: Config banking with tests
    run: cmake -H. -B ${{github.workspace}}/build -DBUILD_TESTS=ON

  - name: Build banking
    run: cmake --build ${{github.workspace}}/build

  - name: Run tests
    run: build/check


  - name: Create coverage directory
    run: mkdir -p coverage

  - name: Set permissions for coverage directory
    run: chmod -R 777 coverage

  - name: Do lcov stuff
    run: lcov -c -d build/CMakeFiles/banking.dir/banking/ --include *.cpp --output-file ./coverage/lcov.info

  - name: Publish to coveralls.io
    uses: coverallsapp/github-action@v1.1.2
    with:
      github-token: ${{ secrets.GITHUB_TOKEN }}
EOF

```

[![Coverage Status](https://coveralls.io/repos/github/Dayinu/lab05/badge.svg?branch=main)](https://coveralls.io/github/Dayinu/lab05?branch=main)

## Links

- [C++ CI: Travis, CMake, GTest, Coveralls & Appveyor](http://david-grs.github.io/cpp-clang-travis-cmake-gtest-coveralls-appveyor/)
- [Boost.Tests](http://www.boost.org/doc/libs/1_63_0/libs/test/doc/html/)
- [Catch](https://github.com/catchorg/Catch2)

```
Copyright (c) 2015-2021 The ISC Authors
```
