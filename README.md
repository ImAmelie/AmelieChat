<div align="center">
  <h1>AmelieChat</h1>
  <p>✨ 音视频即时通讯软件 ✨</p>
  <p>
    <a href="https://raw.githubusercontent.com/ImAmelie/AmelieChat/master/LICENSE">
      <img alt="license" src="https://img.shields.io/github/license/ImAmelie/AmelieChat?style=flat-square">
    </a>
  </p>
</div>


## 客户端

采用 Qt 5 开发，视频模块 [OpenCV](https://opencv.org/)，音频模块 Qt 原生组件。

## 服务器

MySQL：存储注册用户，验证客户端登陆

Redis：保存用户 IP 和 端口，用来进行点对点通信
