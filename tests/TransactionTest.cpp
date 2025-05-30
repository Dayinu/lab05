#include "banking/Account.h"
#include "banking/Transaction.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockAccount : public Account {
public:
    MockAccount(int id, int balance) : Account(id, balance) {}
    MOCK_METHOD(int, GetBalance, (), (const));
    MOCK_METHOD(void, ChangeBalance, (int));
    MOCK_METHOD(void, Lock, ());
    MOCK_METHOD(void, Unlock, ());
};

class MockTransaction : public Transaction {
public:
    MOCK_METHOD(bool, Make, (Account& from, Account& to, int sum));
    MOCK_METHOD(int, fee, (), (const));
};

TEST(TransactionTest, MockTest) {
    MockAccount from(1, 2000);
    MockAccount to(2, 1000);
    MockTransaction tr;
    
    // Настраиваем ожидания
    EXPECT_CALL(tr, fee())
        .WillOnce(::testing::Return(50));
    
    EXPECT_CALL(from, Lock())
        .Times(1);
    
    EXPECT_CALL(to, Lock())
        .Times(1);
    
    EXPECT_CALL(from, GetBalance())
        .WillOnce(::testing::Return(2000));
    
    EXPECT_CALL(from, ChangeBalance(-2050))  // сумма + комиссия
        .WillOnce(::testing::Return());
    
    EXPECT_CALL(to, ChangeBalance(2000))
        .WillOnce(::testing::Return());
    
    EXPECT_CALL(from, Unlock())
        .Times(1);
    
    EXPECT_CALL(to, Unlock())
        .Times(1);
    
    // Выполняем тест
    ASSERT_TRUE(tr.Make(from, to, 2000));
}

TEST(TransactionTest, Banking) {
    const int initial_balance_Alice = 10000;
    const int initial_balance_Bob = 2000;
    const int transaction_fee = 50;

    Account Alice(0, initial_balance_Alice), Bob(1, initial_balance_Bob);
    Transaction test_tran;

    ASSERT_EQ(test_tran.fee(), 1);
    test_tran.set_fee(transaction_fee);
    ASSERT_EQ(test_tran.fee(), transaction_fee);

    ASSERT_THROW(test_tran.Make(Alice, Alice, 2000), std::logic_error);
    ASSERT_THROW(test_tran.Make(Alice, Bob, -100), std::invalid_argument);
    ASSERT_THROW(test_tran.Make(Alice, Bob, 50), std::logic_error);

    if (test_tran.fee() * 2 - 1 >= 200) {
        ASSERT_FALSE(test_tran.Make(Alice, Bob, test_tran.fee() * 2 - 1));
    }

    Alice.Lock();
    ASSERT_THROW(test_tran.Make(Alice, Bob, 2000), std::runtime_error);
    Alice.Unlock();

    ASSERT_TRUE(test_tran.Make(Alice, Bob, 2000));
    ASSERT_EQ(Bob.GetBalance(), initial_balance_Bob + 2000);	
    ASSERT_EQ(Alice.GetBalance(), initial_balance_Alice - 2000 - transaction_fee);

    ASSERT_FALSE(test_tran.Make(Alice, Bob, 8000));
    ASSERT_EQ(Bob.GetBalance(), initial_balance_Bob + 2000);	
    ASSERT_EQ(Alice.GetBalance(), initial_balance_Alice - 2000 - transaction_fee);
}
