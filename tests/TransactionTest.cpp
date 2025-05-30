#include "banking/Account.h"
#include "banking/Transaction.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;
using ::testing::_;

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
    MOCK_METHOD(void, SaveToDataBase, (Account& from, Account& to, int sum), (override));
};

TEST(TransactionTest, MakeTransactionSuccess) {
    MockAccount from(1, 3000);  // Достаточный баланс
    MockAccount to(2, 1000);
    Transaction tr;
    tr.set_fee(50);  // Устанавливаем комиссию

    // Ожидаемые вызовы
    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(3000));
    EXPECT_CALL(to, ChangeBalance(2000)).Times(1);  // Кредитуем получателя
    EXPECT_CALL(from, ChangeBalance(-2050)).Times(1);  // Дебитуем отправителя (сумма + комиссия)
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    ASSERT_TRUE(tr.Make(from, to, 2000));
}

TEST(TransactionTest, MakeTransactionInsufficientFunds) {
    MockAccount from(1, 1000);  // Недостаточный баланс
    MockAccount to(2, 1000);
    Transaction tr;
    tr.set_fee(50);

    EXPECT_CALL(from, Lock()).Times(1);
    EXPECT_CALL(to, Lock()).Times(1);
    EXPECT_CALL(from, GetBalance()).WillOnce(Return(1000));
    EXPECT_CALL(to, ChangeBalance(2000)).Times(1);
    EXPECT_CALL(to, ChangeBalance(-2000)).Times(1);  // Откат транзакции
    EXPECT_CALL(from, Unlock()).Times(1);
    EXPECT_CALL(to, Unlock()).Times(1);

    ASSERT_FALSE(tr.Make(from, to, 2000));
}

TEST(TransactionTest, MakeTransactionInvalidCases) {
    Account alice(1, 1000);
    Account bob(2, 1000);
    Transaction tr;

    // Попытка перевода самому себе
    ASSERT_THROW(tr.Make(alice, alice, 200), std::logic_error);
    
    // Отрицательная сумма
    ASSERT_THROW(tr.Make(alice, bob, -100), std::invalid_argument);
    
    // Слишком маленькая сумма
    ASSERT_THROW(tr.Make(alice, bob, 50), std::logic_error);
    
    // Сумма меньше чем 2*комиссия
    tr.set_fee(60);
    ASSERT_FALSE(tr.Make(alice, bob, 100));
}

TEST(TransactionTest, BankingIntegrationTest) {
    Account alice(1, 10000);
    Account bob(2, 2000);
    Transaction tr;
    tr.set_fee(50);

    // Успешная транзакция
    ASSERT_TRUE(tr.Make(alice, bob, 2000));
    ASSERT_EQ(alice.GetBalance(), 10000 - 2000 - 50);
    ASSERT_EQ(bob.GetBalance(), 2000 + 2000);

    // Неудачная транзакция (недостаточно средств)
    ASSERT_FALSE(tr.Make(alice, bob, 8000));
    ASSERT_EQ(alice.GetBalance(), 10000 - 2000 - 50);  // Баланс не изменился
    ASSERT_EQ(bob.GetBalance(), 2000 + 2000);
}
