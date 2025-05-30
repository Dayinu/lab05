#include <iostream>
#include <Account.h>
#include <Transaction.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(void, Unlock, ());
    MOCK_METHOD(void, Lock, ());
    MOCK_METHOD(int, id, (), (const));
    MOCK_METHOD(void, ChangeBalance, (int diff));
    MOCK_METHOD(int, GetBalance, ());
};

TEST(Account, Balance_ID_Change) {
    MockAccount acc(1, 100);
    // Проверки вызовов методов
    acc.GetBalance();
    acc.id();
    acc.Unlock();
    acc.ChangeBalance(1000);
    acc.GetBalance();
    acc.ChangeBalance(2);
    acc.GetBalance();
    acc.Lock();
}

TEST(Account, Balance_ID_Change_2) {
    Account acc(0, 100);
    EXPECT_THROW(acc.ChangeBalance(50), std::runtime_error);
    acc.Lock();
    acc.ChangeBalance(50);
    EXPECT_EQ(acc.GetBalance(), 150);
    EXPECT_THROW(acc.Lock(), std::runtime_error);
    acc.Unlock();
}


