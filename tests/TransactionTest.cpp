#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../banking/Transaction.h"
#include "../banking/Account.h"

using ::testing::Return;
using ::testing::_;

TEST(TransactionTest, MakeFailsWhenFeeTooHigh) {
    Transaction tr;
    tr.set_fee(100);
    
    MockAccount from(1, 200);
    MockAccount to(2, 100);
    
    EXPECT_FALSE(tr.Make(from, to, 50));
}
