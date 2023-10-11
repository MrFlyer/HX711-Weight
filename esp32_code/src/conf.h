#define CALIBRATION_FACTOR 420.88 //这里需要换成你自己测量之后的数据 测算公式 ： 结果 / 已知物体重量
#define setCalibration false
#define LED1_HX711 13
#define LED2_WIFI 8
#define LED3 4

// HX711配置项
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 18;
//WIFI配置项
const char *ssid = "你的WIFI名字";
const char *password = "你的WIFI密码";
const char *mqttServer = "你的MQTT服务器地址";
const int mqttPort = 1883; //你的MQTT服务器端口
const char *mqttUser = "你的MQTT登录用户名";
const char *mqttPassword = "你的MQTT登录密码";
const char *mqtt_topic = "data";

char message_data[10];
