#include "gtest/gtest.h"

#include "jdcloud_signer/http/URI.h"

using namespace jdcloud_signer;

TEST(URI, CanonicalizeQueryString) {
    const char* testcases[][2] = {
        // TODO: #7
        // {"?a&b", "?a=&b="},
        // {"?a&b=", "?a=&b="},
        // {"?a=&b", "?a=&b="},
        {"?a=&b=", "?a=&b="},
        {"?b=&a=", "?a=&b="},
        // TODO: #50
        {"?a=1&a=2", "?a=1&a=2"},
        // {"?a=2&a=1", "?a=1&a=2"}
    };

    for(int i = 0; i < sizeof(testcases)/sizeof(testcases[0]); ++i) {
        URI url(testcases[i][0]);
        url.CanonicalizeQueryString();
        EXPECT_EQ(url.GetQueryString(), testcases[i][1]);
    }
}
