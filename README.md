# RemoteControl

尝试做一个用于远程控制的简单工具(未完成)

一般为局域网环境下使用(热点)

------

`说明` 使用两种协议 [可选TCP/UDP(默认)]

```sh
   通过在pro中添加 DEFINES += USE_TCP 
```

`说明` 使用两种截屏方式 [可选D3D/GDI(默认，且Qt自带)]

```sh
   通过在pro中添加 DEFINES += USE_GDI 或者 DEFINES += USE_D3D
```
------
使用Qml开发

开发环境windows 7，Qt 5.12.0

------

`注意` 该软件仅用于学习，而不适合作为实际软件使用