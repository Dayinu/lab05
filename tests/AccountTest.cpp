#include "banking/Account.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Throw;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(AccountTests, InitialBalanceIsCorrect) {
    Account acc(24, 42);
    EXPECT_EQ(acc.GetBalance(), 42);
    EXPECT_EQ(acc.id(), 24);
}

TEST(AccountTests, ChangeBalanceWhenLocked) {
    Account acc(24, 42);
    
    acc.Lock();
    EXPECT_NO_THROW(acc.ChangeBalance(958));
    EXPECT_EQ(acc.GetBalance(), 1000);
    
    acc.ChangeBalance(-1000);
    EXPECT_EQ(acc.GetBalance(), 0);
}

TEST(AccountTests, ChangeBalanceFailsWhenUnlocked) {
    Account acc(24, 42);
    EXPECT_THROW(acc.ChangeBalance(100), std::runtime_error);
    EXPECT_EQ(acc.GetBalance(), 42);  // Balance shouldn't change
}

TEST(AccountTests, DoubleLockThrowsException) {
    Account acc(24, 42);
    acc.Lock();
    EXPECT_THROW(acc.Lock(), std::runtime_error);
}

TEST(AccountTests, UnlockWithoutLockThrowsException) {
    Account acc(24, 42);
    EXPECT_THROW(acc.Unlock(), std::runtime_error);
}

TEST(AccountTests, LockUnlockSequenceWorks) {
    Account acc(24, 42);
    
    EXPECT_NO_THROW(acc.Lock());
    EXPECT_NO_THROW(acc.ChangeBalance(100));
    EXPECT_NO_THROW(acc.Unlock());
    EXPECT_THROW(acc.ChangeBalance(100), std::runtime_error);
}

TEST(AccountTests, MockAccountBehavior) {
    MockAccount mockAcc(1, 1000);
    
    EXPECT_CALL(mockAcc, Lock()).Times(1);
    EXPECT_CALL(mockAcc, Unlock()).Times(1);
    EXPECT_CALL(mockAcc, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(mockAcc, ChangeBalance(500)).Times(1);
    
    mockAcc.Lock();
    mockAcc.ChangeBalance(500);
    EXPECT_EQ(mockAcc.GetBalance(), 1000); // Mock возвращает то, что мы настроили
    mockAcc.Unlock();
}

TEST(AccountTests, BalanceChangeOverflowProtection) {
    Account acc(1, INT_MAX);
    acc.Lock();
    EXPECT_THROW(acc.ChangeBalance(1), std::overflow_error);
    acc.Unlock();
    
    Account acc2(1, INT_MIN);
    acc2.Lock();
    EXPECT_THROW(acc2.ChangeBalance(-1), std::underflow_error);
    acc2.Unlock();
}
