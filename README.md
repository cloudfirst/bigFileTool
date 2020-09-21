# bigFileTool
a big file sharing tool based on oxfold infrastructure. 

## 编译BigFileTool
用Qt Creator打开源代码根目录下的bifiletest.pro文件即可。

## 编译webTool
用Qt Creator打开源代码根目录下的3rd/http/civetweb/Qt/CivetWeb.pro文件即可

## 目录说明
- 源代码根目录下的*.h, *.cpp, *.ui文件是bigFileTool的Qt-based UI
- 3rd/http/civetweb/目录下是开源的嵌入式web服务器的代码
- 3rd/http/bin/目录下是嵌入式web服务器在不同平台下的编译结果
- 3rd/oxfold/include/目录下是libzt动态链接库的头文件
- 3rd/oxfold/lib/目录下是libzt在不同平台下编译好的动态链接库文件
- 3rd/oxfold/src/目录下是封装了libzt API的C函数，负责启动和终止libzt服务.这些C函数在CivetWeb的main()函数里面调用。
