1、本代码在Adruino mega2560上烧写，用Arduino IDE 下载 Arduino mega2560 开发板;  
2、因gps和nb模块都是uart，正好mega2560有三个硬串口很适合调试，之前用esp8266 用软串口模拟一路但没有成功;  
3、随后打算用stm32F103C8T6这个板子做一遍;  
4、nb模块用的移远BC35-G 全网通模块，配合一块移动nb卡；  
5、IOT平台可以自己搭一套thingsboard 的IOT开源物联网平台，但它默认没有udp通道所以还需要用nodejs写一个udp转mqtt，这样把udp数据上行到iot平台；  
6、也可以用谷雨的iot平台，这样省去自己搭建平台；  
7、烧写代码前先通过usb转ttl模块 把nb模块调通在整体运行；  
