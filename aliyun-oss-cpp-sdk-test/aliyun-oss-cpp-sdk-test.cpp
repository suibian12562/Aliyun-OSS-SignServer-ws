#include <alibabacloud/oss/OssClient.h>
using namespace AlibabaCloud::OSS;

int main(void)
{
    /* 初始化OSS账号信息。*/
    /* 阿里云账号AccessKey拥有所有API的访问权限，风险很高。强烈建议您创建并使用RAM用户进行API访问或日常运维，请登录RAM控制台创建RAM用户。*/
    std::string AccessKeyId = "";
    std::string AccessKeySecret = "";
    /* yourEndpoint填写Bucket所在地域对应的Endpoint。以华东1（杭州）为例，Endpoint填写为https://oss-cn-hangzhou.aliyuncs.com。*/
    std::string Endpoint = "";
    /* 填写Bucket名称，例如examplebucket。*/
    std::string BucketName = "";
    /* 填写Object完整路径，完整路径中不能包含Bucket名称，例如exampledir/exampleobject.txt。*/
    std::string GetobjectUrlName = "";

    /* 初始化网络等资源。*/
    InitializeSdk();

    ClientConfiguration conf;
    OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);

    /* 设置签名URL有效时长。*/
    std::time_t t = std::time(nullptr) + 20;
    /* 生成签名URL。*/
    auto genOutcome = client.GeneratePresignedUrl(BucketName, GetobjectUrlName, t, Http::Get);
    if (genOutcome.isSuccess()) {
        std::cout << "GeneratePresignedUrl success, Gen url:" << genOutcome.result().c_str() << std::endl;
    }
    else {
        /* 异常处理。*/
        std::cout << "GeneratePresignedUrl fail" <<
            ",code:" << genOutcome.error().Code() <<
            ",message:" << genOutcome.error().Message() <<
            ",requestId:" << genOutcome.error().RequestId() << std::endl;
        return -1;
    }

    /* 释放网络等资源。*/
    ShutdownSdk();
    return 0;
}