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
