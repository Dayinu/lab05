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
                                                                                     
$ cat << EOF > Test.cpp
#include <Account.h>
#include <Transaction.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::Throw;
using ::testing::_;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_CONST_METHOD0(GetBalance, int());
    MOCK_METHOD1(ChangeBalance, void(int));
    MOCK_METHOD0(Lock, void());
    MOCK_METHOD0(Unlock, void());
};

TEST(AccountTest, MockLocker) {
    MockAccount acc(0, 1111);
    EXPECT_CALL(acc, Lock()).Times(2);
    EXPECT_CALL(acc, Unlock()).Times(1);
    acc.Lock();
    acc.Lock();
    acc.Unlock();
}

TEST(AccountTest, MockMethods) {
    MockAccount acc(1, 1000);
    EXPECT_CALL(acc, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(acc, Lock()).Times(1);
    EXPECT_CALL(acc, ChangeBalance(500)).Times(1);
    EXPECT_CALL(acc, Unlock()).Times(1);

    ASSERT_EQ(acc.GetBalance(), 1000);
    acc.Lock();
    acc.ChangeBalance(500);
    acc.Unlock();
}

TEST(AccountTest, Banking) {
    Account acc(0, 0);
    ASSERT_EQ(acc.GetBalance(), 0);
    ASSERT_THROW(acc.ChangeBalance(100), std::runtime_error);
    
    acc.Lock();
    ASSERT_NO_THROW(acc.ChangeBalance(100));
    ASSERT_EQ(acc.GetBalance(), 100);
    ASSERT_THROW(acc.Lock(), std::runtime_error);
    
    acc.Unlock();
    ASSERT_THROW(acc.ChangeBalance(100), std::runtime_error);
}

TEST(TransactionTest, ConstructAndPositive) {
    Transaction txn;
    ASSERT_EQ(txn.fee(), 1);

    txn.set_fee(32);
    ASSERT_EQ(txn.fee(), 32);

    Account from(0, 6132);
    Account to(1, 2133);
    
    ASSERT_TRUE(txn.Make(from, to, 100));
    ASSERT_EQ(from.GetBalance(), 6132 - 100 - 32);
    ASSERT_EQ(to.GetBalance(), 2133 + 100);
}

TEST(TransactionTest, Negative) {
    Transaction txn;
    txn.set_fee(51);
    
    Account acc1(0, 10);
    Account acc2(1, 1000);

    ASSERT_THROW(txn.Make(acc1, acc1, 0), std::logic_error);
    ASSERT_THROW(txn.Make(acc1, acc2, -100), std::invalid_argument);
    ASSERT_THROW(txn.Make(acc1, acc2, 50), std::logic_error);
    ASSERT_FALSE(txn.Make(acc1, acc2, 100));
    
    txn.set_fee(10);
    ASSERT_FALSE(txn.Make(acc1, acc2, 100));
}

TEST(TransactionTest, Banking) {
    const int initial_balance_from = 10000;
    const int initial_balance_to = 2000;
    const int fee = 50;

    Account from(0, initial_balance_from);
    Account to(1, initial_balance_to);
    Transaction txn;

    ASSERT_EQ(txn.fee(), 1);
    txn.set_fee(fee);
    ASSERT_EQ(txn.fee(), fee);

    ASSERT_THROW(txn.Make(from, from, 2000), std::logic_error);
    ASSERT_THROW(txn.Make(from, to, -100), std::invalid_argument);
    ASSERT_THROW(txn.Make(from, to, 50), std::logic_error);

    if (txn.fee() * 2 - 1 >= 200) {
        ASSERT_FALSE(txn.Make(from, to, txn.fee() * 2 - 1));
    }

    from.Lock();
    ASSERT_THROW(txn.Make(from, to, 2000), std::runtime_error);
    from.Unlock();

    ASSERT_TRUE(txn.Make(from, to, 2000));
    ASSERT_EQ(to.GetBalance(), initial_balance_to + 2000);
    ASSERT_EQ(from.GetBalance(), initial_balance_from - 2000 - fee);

    ASSERT_FALSE(txn.Make(from, to, 8000));
    ASSERT_EQ(to.GetBalance(), initial_balance_to + 2000);
    ASSERT_EQ(from.GetBalance(), initial_balance_from - 2000 - fee);
}
EOF

$cd ..
                                                                                     
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
