#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "banking/Account.h"

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(AccountTest, BalanceTest) {
    MockAccount acc(1, 1000);
    
    EXPECT_CALL(acc, GetBalance())
        .Times(1)
        .WillOnce(testing::Return(1000));
    
    EXPECT_CALL(acc, Lock())
        .Times(1);
    
    EXPECT_CALL(acc, ChangeBalance(1000))
        .Times(1);
}
