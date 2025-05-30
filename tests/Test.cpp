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
