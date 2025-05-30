cmake_minimum_required(VERSION 3.10)
project(lab05)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основная библиотека
add_library(banking
    banking/Account.cpp
    banking/Transaction.cpp
)
target_include_directories(banking PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/banking
)

# Настройка тестов
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Настройка покрытия кода
    add_compile_options(--coverage)
    add_link_options(--coverage)

    # Подключаем Google Test
    add_subdirectory(third-party/gtest)

    # Тестовый исполняемый файл
    add_executable(tests
        tests/AccountTest.cpp
        tests/TransactionTest.cpp
    )

    # Директории для включения
     target_include_directories(tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}         # Корень проекта (для #include "banking/...")
        ${CMAKE_CURRENT_SOURCE_DIR}/banking # Прямой путь к банковским заголовкам
    )

    # Связываем зависимости
    target_link_libraries(tests
        banking
        gtest_main
        gmock_main
    )

    # Добавляем в CTest
    enable_testing()
    add_test(NAME lab05_tests COMMAND tests)
endif()
