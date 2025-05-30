#include <Account.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(AccountTest, MockTest) {
    MockAccount acc(1, 1000);
    
    EXPECT_CALL(acc, GetBalance())
        .Times(1)
        .WillOnce(::testing::Return(1000));
    
    EXPECT_CALL(acc, Lock())
        .Times(1);
    
    EXPECT_CALL(acc, ChangeBalance(500))
        .Times(1);
    
    EXPECT_CALL(acc, Unlock())
        .Times(1);
    
    ASSERT_EQ(acc.GetBalance(), 1000);
    acc.Lock();
    acc.ChangeBalance(500);
    acc.Unlock();
}

TEST(AccountTest, Banking) {
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
