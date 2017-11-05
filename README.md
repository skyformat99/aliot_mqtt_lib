# aliot_mqtt_lib
aliot Direct connection demo

//代码基于mico系统搭建，使用了新的sdk。3.5版本 。请您参照庆科说明获取最新的   mico-os文件夹
//文件使用utf-8  不然会出现乱码
//测试板子EMW3031_kit
//您可以用命令创建一个helloworld工程，然后导入我的压缩包
//mqtt直连的代码在组件sioiot_aliot_z文件夹下
//首先根据自己的套件数据到sioiot_conifg.h 里面去配置
//因为集成了激活demo，如果需要测试的话需要到可以到 sioiot_activate.c 里面找到ceshi函数，修改对应参数
//实现了ota功能，目前测试完好
//设备第一次执行代码会重启两次
//关于影子，我写好的初始化和获取部分，目前代码不全面，我对于影子的理解就是一个缓存，根据自己实际的需要，实际写就好，没必要集成全部的
//如果需要交流  可以加群290239969或者568153758
//也可以发邮件lqq@sioiot.com
