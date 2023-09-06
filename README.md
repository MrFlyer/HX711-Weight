# HX711耗材检测

# 我的技术技术很一般代码更多是到处借鉴整合而来，请多包容指正，<u>***并不能保证在你那边运行没有BUG***</u>

## 我做了什么

- 实现了基础功能

- 在相对没有bug的情况下正常运行了大概一个小时

- 完成了初步💩山的堆积，并且成功让他跑了起来

## 你需要做什么

- 根据我的教程使用Docker/宿主机进行安装MQTT服务端

- 在源码的对应位置修改为你自己的WIFI SSID以及相对应的密码，并且配置好MQTT服务器的链接

- 开始使用

- 如果对你有帮助的话请给star！对于源码哪里有问题欢迎提issue

## 让我们开始吧

#### 服务器端MQTT环境搭建

1. 创建docker-compose
   
   - 首先确保你已经正确安装好了docker以及docker-compose，至于docker的安装方法这里不会赘述，可以前往官网进行查看并安装到你的linux系统（最好不要安装在klipper端，以免额外增加性能损耗，降低打印效果）。
   
   - 在你的linux服务器下任意目录创建`docker-compose.yml`并使用你擅长的文件编辑器写入如下
     
     ```docker
     version: '3'
     
     services:
       emqx1:
         image: emqx:5.1.3
         container_name: emqx1
         environment:
         - "EMQX_NODE_NAME=emqx@node1.emqx.io"
         - "EMQX_CLUSTER__DISCOVERY_STRATEGY=static"
         - "EMQX_CLUSTER__STATIC__SEEDS=[emqx@node1.emqx.io,emqx@node2.emqx.io]"
         healthcheck:
           test: ["CMD", "/opt/emqx/bin/emqx ctl", "status"]
           interval: 5s
           timeout: 25s
           retries: 5
         networks:
           emqx-bridge:
             aliases:
             - node1.emqx.io
         ports:
           - 1883:1883
           - 8083:8083
           - 8084:8084
           - 8883:8883
           - 18083:18083 
         # volumes:
         #   - $PWD/emqx1_data:/opt/emqx/data
     
       emqx2:
         image: emqx:5.1.3
         container_name: emqx2
         environment:
         - "EMQX_NODE_NAME=emqx@node2.emqx.io"
         - "EMQX_CLUSTER__DISCOVERY_STRATEGY=static"
         - "EMQX_CLUSTER__STATIC__SEEDS=[emqx@node1.emqx.io,emqx@node2.emqx.io]"
         healthcheck:
           test: ["CMD", "/opt/emqx/bin/emqx ctl", "status"]
           interval: 5s
           timeout: 25s
           retries: 5
         networks:
           emqx-bridge:
             aliases:
             - node2.emqx.io
         # volumes:
         #   - $PWD/emqx2_data:/opt/emqx/data
     
     networks:
       emqx-bridge:
         driver: bridge
     ```

2. 通过命令行切换 `docker-compose.yml` 文件所在目录，然后输入以下命令启动 EMQX 集群:

```shell
docker-compose up -d
```

3. 通过局域网IP+端口并且以可以成功访问则为MQTT服务器部署成功

​ 默认用户名及密码：

​ `admin`

​ `public`

#### 修改ESP32端源码以及客户端源码以适配你的局域网环境

###### ESP32端

1. 推荐使用安装好有PlatformIO的VSCode直接在插件内部打开工程（具体安装方法直接百度即可非常之简单没有坑）等待所有配置加载完成并且下载好所需要的开发板库以及轮子

2. 进入到`ESP源码/src`目录下找到main.cpp，根据提示进行针对你自己的网络还近进行修改

```cpp
const char *ssid = "你的WIFI名字";
const char *password = "你的WIFI密码";
const char *mqttServer = "你的MQTT服务器地址";
const int mqttPort = 1883; //你的MQTT服务器端口
const char *mqttUser = "你的MQTT登录用户名";
const char *mqttPassword = "你的MQTT登录密码";
const char *mqtt_topic = "data";
```

3. 在第一次拿到HX711的时候注意将设置为True并且根据以下刷入esp32并且根据串口数据提示进行去皮计算，在计算完成之后改成false后即可

```cpp
#define setCalibration true
```

- 通过(reading)/(known weight)的方式计算出CALIBRATION_FACTOR并将其填入即可

```cpp
#define CALIBRATION_FACTOR 420.88 //这里需要换成你自己测量之后的数据 测算公式 ： 结果 / 已知物体重量
```

- 在保证修改完成并且无误之后可以插上esp32并且来到VSCode左下角，选择PlatformIO:Upload进行编译上传

- 根据源码中的引脚定义，进行esp32模块与hx711的链接，这里推荐使用5v供给HX711，来降低对于AMS1117的负载

```cpp
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 18;
```

##### 打印机端

1. 进入到`weight_use`下并且找到main.py修改如下关键地方

```python
REPORT_TOPIC = 'data'  # 主题
file_name = "weight"
server_add = "你的mqtt服务器地址"
```

2. 通过使用pip进行安装`paho-mqtt==1.6.1`也可以在对应目录下使用如下方式进行安装

```shell
pip install -r requirements.txt
```

3. 通过FTP等任意你熟悉的方式，熟悉的软件将`weight_use`传入打印机上位机并通过如下方式启动 <u>*注意这里使用python还是python3取决于你自己的环境设置*</u>

```shell
python3/python main.py
```

4. 在printer.cfg之中添加以下

```tsconfig
[temperature_sensor 剩余耗材重量(g)]           #重量显示
sensor_type: temperature_host
sensor_path: /home/klipper/weight_use/weight #注意更换成你自己的文件位置
min_temp: -100            #下限
max_temp: 11000          #上限
```

### 没了就这点儿玩意儿，其实也不是非得需要再弄一个服务器这样，市面上目前有很多提供服务的服务商，直接购买他们的服务再进行对接可以更好的更高效的实现效果。

### 以后可能会实现：

- 尽可能的减少ESP32端的功耗，实现除了使用电源dcdc降压以外的另外其他供电方式

- 增添其他外设，试图整点花活儿

- 缓慢更新出WebUI配网配置Mqtt的方式
