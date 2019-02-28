#include "gtest/gtest.h"

#include "jdcloud_signer/util/crypto/Sha256.h"

using namespace jdcloud_signer;

TEST(Sha256, Calculate_String) {
    Sha256 sha256;
    auto result = sha256.Calculate("");
    ASSERT_TRUE(result.IsSuccess());
}
