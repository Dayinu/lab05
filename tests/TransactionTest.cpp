#include <gtest/gtest.h>
#include "Transaction.h"
#include "Account.h"

using ::testing::Return;
using ::testing::Throw;
using ::testing::_;

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account&, Account&, int), (override));
};

TEST(TransactionTest, MakeTransferSuccess) {
    MockAccount from(1, 2000);
    MockAccount to(2, 1000);
    MockTransaction tr;
    
    EXPECT_CALL(from, Lock());
    EXPECT_CALL(to, Lock());
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(2000));
    EXPECT_CALL(from, ChangeBalance(-121)).WillOnce(Return());
    EXPECT_CALL(to, ChangeBalance(120)).WillOnce(Return());
    
    tr.set_fee(1);
    bool result = tr.Make(from, to, 120);
    
    EXPECT_TRUE(result);
}

TEST(TransactionTest, InvalidSameAccount) {
    MockAccount acc(1, 1000);
    Transaction tr;
    EXPECT_THROW(tr.Make(acc, acc, 100), std::logic_error);
}

TEST(TransactionTest, InvalidNegativeSum) {
    MockAccount from(1, 1000);
    MockAccount to(2, 1000);
    Transaction tr;
    EXPECT_THROW(tr.Make(from, to, -50), std::invalid_argument);
}
