#include "banking/Account.h"
#include "banking/Transaction.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(void, SaveToDataBase, (Account&, Account&, int), (override));
};

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const, override));
    MOCK_METHOD(void, ChangeBalance, (int), (override));
    MOCK_METHOD(void, Lock, (), (override));
    MOCK_METHOD(void, Unlock, (), (override));
};

TEST(TransactionTests, SameAccountsThrowLogicError) {
    Account acc(42, 1000);
    Transaction tr;
    EXPECT_THROW(tr.Make(acc, acc, 200), std::logic_error);
}

TEST(TransactionTests, NegativeSumThrowInvalidArgument) {
    Account from(42, 1000);
    Account to(24, 500);
    Transaction tr;
    EXPECT_THROW(tr.Make(from, to, -200), std::invalid_argument);
}

TEST(TransactionTests, SmallSumThrowLogicError) {
    Account from(42, 1000);
    Account to(24, 500);
    Transaction tr;
    EXPECT_THROW(tr.Make(from, to, 50), std::logic_error);
}

TEST(TransactionTests, SuccessfulTransactionUpdatesBalances) {
    Account from(42, 1000);
    Account to(24, 500);
    Transaction tr;
    
    EXPECT_TRUE(tr.Make(from, to, 300));
    EXPECT_EQ(from.GetBalance(), 1000 - 300 - tr.fee());
    EXPECT_EQ(to.GetBalance(), 500 + 300);
}

TEST(TransactionTests, FailedTransactionDueToInsufficientFunds) {
    Account from(42, 350);
    Account to(24, 500);
    Transaction tr;
    tr.set_fee(51);
    
    EXPECT_FALSE(tr.Make(from, to, 300));
    EXPECT_EQ(from.GetBalance(), 350);  // Balance shouldn't change
    EXPECT_EQ(to.GetBalance(), 500);    // Balance shouldn't change
}

TEST(TransactionTests, SaveToDatabaseCalled) {
    MockTransaction tr;
    Account from(1, 1000);
    Account to(2, 500);
    
    EXPECT_CALL(tr, SaveToDataBase(_, _, _)).Times(1);
    tr.SaveToDataBase(from, to, 100);
}

TEST(TransactionTests, LockUnlockCalledDuringTransaction) {
    MockAccount from(1, 1000);
    MockAccount to(2, 500);
    Transaction tr;
    
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);
    
    tr.Make(from, to, 200);
}

TEST(TransactionTests, TransactionRollbackWhenDebitFails) {
    MockAccount from(1, 100);
    MockAccount to(2, 500);
    Transaction tr;
    tr.set_fee(50);
    
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(100));
    EXPECT_CALL(to, ChangeBalance(200)).Times(1); 
    EXPECT_CALL(to, ChangeBalance(-200)).Times(1); 
    
    EXPECT_FALSE(tr.Make(from, to, 200));
}
