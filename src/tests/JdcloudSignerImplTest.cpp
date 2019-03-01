#include "gtest/gtest.h"

#include "jdcloud_signer/JdcloudSignerImpl.h"

using namespace jdcloud_signer;
using namespace std;

HttpRequest BuildAndSignRequestFromUrl(const string& url) {
    HttpRequest request(url, HttpMethod::HTTP_GET);
    Credential credential("ak", "sk");
    JdcloudSignerImpl signer(credential, "vm", "cn-north-1");
    DateTime now(INT64_C(1234567890000));
    string uuid("uuid");
    bool result = signer.SignRequest(request, now, uuid);
    assert(result);
    return request;
}

TEST(JdcloudSignerImpl, SignRequest) {
    auto request = BuildAndSignRequestFromUrl("http://vm.cn-north-1.jdcloud.net/");
    // TODO: #48
    // ASSERT_TRUE(request.HasHeader("User-Agent"));
    ASSERT_EQ(request.GetHeaders().size(), 4);
    ASSERT_EQ(request.GetHeaderValue("x-jdcloud-nonce"), "uuid");
    ASSERT_EQ(request.GetHeaderValue("x-jdcloud-date"), "20090213T233130Z");
    ASSERT_EQ(request.GetHeaderValue("authorization"), "JDCLOUD2-HMAC-SHA256 Credential=ak/20090213/cn-north-1/vm/jdcloud2_request, SignedHeaders=host;x-jdcloud-date;x-jdcloud-nonce, Signature=43a76892222031f847c00c56cac8d4d2dc179a4e75a37868d09a5300435f4054");
    ASSERT_EQ(request.GetHeaderValue("host"), "vm.cn-north-1.jdcloud.net");
}

TEST(JdcloudSignerImpl, SignRequestWithEmptyQueryValue) {
    auto auth1 = BuildAndSignRequestFromUrl("http://vm.cn-north-1.jdcloud.net/?a=&b=").GetHeaderValue("authorization");
    // auto auth2 = BuildAndSignRequestFromUrl("http://vm.cn-north-1.jdcloud.net/?a&b=").GetHeaderValue("authorization");
    // auto auth3 = BuildAndSignRequestFromUrl("http://vm.cn-north-1.jdcloud.net/?a=&b").GetHeaderValue("authorization");
    // auto auth4 = BuildAndSignRequestFromUrl("http://vm.cn-north-1.jdcloud.net/?a&b").GetHeaderValue("authorization");
    auto auth5 = BuildAndSignRequestFromUrl("http://vm.cn-north-1.jdcloud.net/?b=&a=").GetHeaderValue("authorization");
    // EXPECT_EQ(auth1, auth2);
    // EXPECT_EQ(auth1, auth3);
    // EXPECT_EQ(auth1, auth4);
    EXPECT_EQ(auth1, auth5);
}
