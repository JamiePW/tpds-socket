# tpds-socket  
### 2022-8-7  
针对simpleSocket3、4进行正式实验前的优化，主要内容如下：  
1、扩大buffer的体积至32kB，方便对较大的数据进行测试；  
2、在common.h文件中以宏的形式规定object和data的数据结构大小，方便调整大小以测试不同的数据集；  
3、simpleSocket3中D在传输状态时增加了查找哈希表一步；  
4、simpleSocket3中对A增加了额外的对与D进行的状态对象传输过程的时间测量；  

### 2022-8-5  
第一次实验参数：  
MAXOBJ = 513  
MAXDATA = 1025  
DATALEN = 513  
OBJLEN分别设定为1k，2k，4k，8k，16k，每个程序每种情况测量5组数据

### 2022-8-4  
对simpleSocket4代码部分细节进行了优化，内容如下：  
1、将存储state和data的二维数组大小以宏的形式规定，便于调整  
2、C处的随机字符串于初始化时自动生成，节约时间  
3、接收消息时避免多次复制  

### 2022-8-3  
针对验证OpenNF论文的核心机制编写了simpleSocket4，主要框架与simpleSocket3相同，针对新需求增加了若干种消息结构，并增加了随机生成字符串模拟流量的功能。  

### 2022-7-29  
对simpleSocket3代码进行了优化，主要包括以下内容：  
1、针对不同种类消息设计不同的消息结构；  
2、建立连接的时间计算在内；  
3、正式计时时要去掉打印消息环节；  
4、实例大字符串内容采用数组或哈希表存储；  

### 2022-7-27  
重写了simpleSocket3，将连接、监听、发送消息等主要功能封装在了common.h头文件中，主程序代码更加简洁清晰。  
改进方向：  

### 2022-7-25  
第二版代码simpleSocket3增强了复用性，服务器和客户端的功能整合在一个代码文件中，可以根据需要选择不同的工作模式；目前，此版代码已经复现了基本的进程间通信功能，下一步计划设计标准的通信消息格式；  

### 2022-7-24  
第一版代码simpleSocket2可以实现基本的通信流程，但还比较粗糙，代码复用性较差，数据结构也没有系统的设计，这些问题我会在第二版中改进。  
