# simpleSocket4  
使用方法与simpleSocket3基本相同。  
注：  
1、随机字符串生成需要用到C++ 11标准，因此编译时需增加参数-std=c++11  
2、正式测试时需将打印消息操作去除以确保测量时间准确  
3、common.h文件中的MAXOBJ和MAXDATA分别决定了转移的状态数量和流量数量，可根据测量需求适当调整  
4、common.h文件中的OBJLEN和DATALEN分别决定了转移的状态长度和流量长度，可根据测量需求适当调整  
