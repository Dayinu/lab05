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
cmake_minimum_required(VERSION 3.10)
project(BankingProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(banking
    banking/Account.cpp
    banking/Transaction.cpp
)

target_include_directories(banking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/banking
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
EOF
                                                                                     
$ mkdir -p tests   
                                                                                     
$ cd tests                   
                                                                                     
$ cat << EOF > AccountTest.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../banking/Account.h"

using ::testing::Return;
using ::testing::_;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(AccountTest, LockThrowsIfAlreadyLocked) {
    MockAccount acc(1, 1000);
    EXPECT_CALL(acc, Lock()).WillOnce(testing::Throw(std::runtime_error("already locked")));
    ASSERT_THROW(acc.Lock(), std::runtime_error);
}
EOF

                                                                                     
$ cat << EOF > TransactionTest.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../banking/Transaction.h"
#include "../banking/Account.h"

using ::testing::Return;
using ::testing::_;

TEST(TransactionTest, MakeFailsWhenFeeTooHigh) {
    Transaction tr;
    tr.set_fee(100);
    
    MockAccount from(1, 200);
    MockAccount to(2, 100);
    
    EXPECT_FALSE(tr.Make(from, to, 50));
}
EOF
                                                                                     
$ mkdir -p .github/workflows
                                                                                     
$ cat << EOF > .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake libgtest-dev lcov
        
    - name: Build and test
      run: |
        mkdir build
        cd build
        cmake ../banking -DBUILD_TESTING=ON -DCMAKE_CXX_FLAGS="--coverage"
        cmake --build .
        ctest --output-on-failure
        
    - name: Upload coverage
      uses: coverallsapp/github-action@v2
      with:
        github-token: \${{ secrets.GITHUB_TOKEN }}
        path-to-lcov: build/coverage.info
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
