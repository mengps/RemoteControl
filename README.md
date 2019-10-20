# RemoteControl

尝试做一个远程控制的简单工具

一般为局域网环境下使用(热点)

------

`说明` 使用两种协议传输 [可选TCP/UDP(默认)]

```
   默认使用UDP来传输屏幕图像，额外的TCP连接来传输EVENT，包括连接状态

   要使用TCP，通过在pro中添加 DEFINES += USE_TCP
```

`说明` 使用三种截屏方式 [可选DXGI/D3D/GDI(默认，且Qt自带)]

```
   通过在pro中添加:

   DEFINES += USE_DXGI 或者 DEFINES += USE_GDI 或者 DEFINES += USE_D3D

   因为DXGI仅在Windows 8以上提供，考虑到Windows 8的使用量，所以Windows 10以上可使用
```
------
### 关于Windows和Android

要知道的是，因为不能完全跨平台，某些功能难以实现(水平不够)

并且界面也需要做一些调整，很麻烦

------

### 开发环境

使用Qt/Qml开发

Windows 10，Qt 5.13.0

基本未使用高版本特性，保证`Qt Version >= 5.0`即可，建议是`Qt 5.5 ~ 5.7`

------

### 后续改进

 - 使用RTP传输

 - 不再传输图像，而是码流

------

### 许可证

   使用 `MIT LICENSE`

------

`注意` 目前传输的不是码流而是JPG图像

`注意` 该软件仅用于学习，而不适合作为实际软件使用