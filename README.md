# Aliyun-OSS-GeneratePresignedUrl-ws

提供一个websocket API,传入要访问的私有bucket中的文件生成签名URL并返回签名URL.计划包含客户端验证.

编译依赖
- [zaphoyd/ _websocketpp_](https://github.com/zaphoyd/websocketpp)
- [nlohmann/_json_](https://github.com/nlohmann/json)
- [aliyun/_aliyun_-_oss_-cpp-sdk](https://github.com/aliyun/aliyun-oss-cpp-sdk)
使用任意编译器编译即可.