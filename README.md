### 人脸识别签到系统
#### 文件说明
- SignSystemModule为识别系统的整体部分，它使用flask框架搭建网站，提供了常见的增删改查功能。  
- FaceRecognitionModule为人脸识别签到模块。其中client可部署在有摄像头的机器上，进行人脸识别；loadface和server部署在服务器上，loadface用于训练识别模型，server用于相应client的请求，将已签到的学生记入数据库。
#### 环境/依赖
- SignSystemModule:  
pycharm、flask、python、MySQL5.7等  
- FaceRecognitionModule:  
ubuntu(linux)、opencv、boost、MySQL5.7、MySQL c api、cmake3.1、C++11等

#### 使用
1. 在MySQL中，新建一个数据库，按照model.py创建相应的表。  
1. 在Pycharm中打开SignSystemModule，安装相关的包，并修改config.py文件中的数据库信息，然后执行run.py  
1. 在Ubuntu(linux)中下载opencv、opencv_contrib、boost、libmysqlclient，并在client、loadface、server中的CMameLists.txt中指明上述库所在对的目录，接着重新编译上述三个文件，然后调用相应的可执行文件即可。
#### 注意
- 如何准备opencv：下载opencv和opencv_contrib，将opencv_contrib拷贝到opencv目录下，在opencv目录中新建build文件夹，在build文件夹中对opencv和opencv_contrib重新编译
- FaceRecognitionModule中的server使用本地的8001端口号默认使用本地数据库，client默认连接到127.0.0.1:8001。你可以修改源码并重新编译来支持远程连接
- 如果你希望数据库允许远程连接或存储中文，记到进行相关配置
- 这个程序中的一些文件文件地址直接写死在源码中，如果运行时发现找不到文件，你可以首先排查文件存放地址是否正确，当然你也可以修改源代码。
