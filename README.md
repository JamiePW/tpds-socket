# tpds-socket  
#### 2022-7-29  
对simpleSocket3代码进行了优化，主要包括以下内容：  
1、针对不同种类消息设计不同的消息结构；  
2、建立连接的时间计算在内；  
3、正式计时时要去掉打印消息环节；  
4、实例大字符串内容采用数组或哈希表存储；  

#### 2022-7-27  
重写了simpleSocket3，将连接、监听、发送消息等主要功能封装在了common.h头文件中，主程序代码更加简洁清晰。  
改进方向：  


#### 2022-7-25  
第二版代码simpleSocket3增强了复用性，服务器和客户端的功能整合在一个代码文件中，可以根据需要选择不同的工作模式；目前，此版代码已经复现了基本的进程间通信功能，下一步计划设计标准的通信消息格式；  

#### 2022-7-24  
第一版代码simpleSocket2可以实现基本的通信流程，但还比较粗糙，代码复用性较差，数据结构也没有系统的设计，这些问题我会在第二版中改进。  
