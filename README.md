# Aliyun-OSS-GeneratePresignedUrl-ws
[![.github/workflows/cmake-linux.yml](https://github.com/suibian12562/Aliyun-OSS-SignServer-ws/actions/workflows/cmake-linux.yml/badge.svg?branch=main)](https://github.com/suibian12562/Aliyun-OSS-SignServer-ws/actions/workflows/cmake-linux.yml)
[![.github/workflows/cmake-windows.yml](https://github.com/suibian12562/Aliyun-OSS-SignServer-ws/actions/workflows/cmake-windows.yml/badge.svg)](https://github.com/suibian12562/Aliyun-OSS-SignServer-ws/actions/workflows/cmake-windows.yml)  
提供一个websocket API,传入要访问的私有bucket中的文件生成签名URL并返回签名URL.计划包含客户端验证.

**编译**  
使用[microsoft/_vcpkg_](https://github.com/microsoft/vcpkg)管理包,为此编译前你需使用此命令安装依赖
```BASH
vcpkg install
```
然后使用Cmake进行编译
```SHELL
cmake -B build
cmake --build build
```
***
**TODO**  
- [x] 签名有效时间  
- [ ] 人机验证  
- [ ] Bucket权限管理  
***
**配置**  
程序在第一次启动时会在目录下生成一个config.json文件,内容如下
```json
{
    "AccessKeyId": "your_access_key",
    "AccessKeySecret": "your_access_secret",
    "port": 1145,
    "sign_time": 40
}
```
将其中的配置替换为你自己的数值.
***
**使用**  
程序会开启一个在设定端口的Websocket服务器
向此端口传入
```JSON
{
        "_Endpoint": endpoint,
        "_Bucket": bucket,
        "_GetobjectUrlName": getObjectUrlName
      }
```
将会收到签名后的链接以供下载
***
**典型前端**  
```HTML
<!DOCTYPE html>
<html>
<head>
  <title>WebSocket Example</title>
</head>
<body>

<script>
  window.onload = function() {
    // 解析URL中的查询参数
    const urlParams = new URLSearchParams(window.location.search);
    const endpoint = urlParams.get('_Endpoint');
    const bucket = urlParams.get('_Bucket');
    const getObjectUrlName = urlParams.get('_GetobjectUrlName');

    // 检查参数是否合法
    if (!endpoint || !bucket || !getObjectUrlName) {
      showError('Invalid parameters. Please provide all required parameters.');
      return;
    }

    const socket = new WebSocket('ws://127.0.0.1:1145');

    socket.onopen = function (event) {
      console.log('WebSocket connected.');

      // 构造参数对象
      const params = {
        "_Endpoint": endpoint,
        "_Bucket": bucket,
        "_GetobjectUrlName": getObjectUrlName
      };

      // 将参数对象转换成 JSON 字符串
      const jsonParams = JSON.stringify(params);

      // 发送参数到服务器
      socket.send(jsonParams);
    };

    socket.onmessage = function (event) {
      // 接收服务器返回的数据
      const receivedData = event.data;

      // 判断接收到的数据是否是链接，如果是则重定向
      if (isURL(receivedData)) {
        window.location.href = receivedData;
      } else {
        displayReceivedData(receivedData);
      }
    };

    socket.onclose = function (event) {
      console.log('WebSocket closed.');
    };

    socket.onerror = function (event) {
      console.error('WebSocket error: ', event);

      // 显示错误提示
      showError('WebSocket connection error. Please try again later.');
    };
  };

  function displayReceivedData(data) {
    const receivedDataDiv = document.createElement('div');
    receivedDataDiv.textContent = 'Received data: ' + data;
    document.body.appendChild(receivedDataDiv);
  }

  function showError(message) {
    const errorMessage = document.createElement('div');
    errorMessage.textContent = message;
    errorMessage.style.color = 'red';
    document.body.appendChild(errorMessage);
  }

  function isURL(str) {
    const urlPattern = /^(https?|ftp):\/\/[^\s/$.?#].[^\s]*$/i;
    return urlPattern.test(str);
  }
</script>

</body>
</html>

```
使用方法如下
```URL
http://example.com?_Endpoint=xxx&_Bucket=xxx&_GetobjectUrlName=xxx
```