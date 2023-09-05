#include "HX711.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define CALIBRATION_FACTOR 420.88

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 5;
const int LOADCELL_SCK_PIN = 18;

const char *ssid = "你的WIFI名字";
const char *password = "你的WIFI密码";
const char *mqttServer = "你的MQTT服务器地址";
const int mqttPort = 1883; //你的MQTT服务器端口
const char *mqttUser = "你的MQTT登录用户名";
const char *mqttPassword = "你的MQTT登录密码";
const char *mqtt_topic = "data";

char message_data[10];

HX711 scale;
WiFiClient espClient;
PubSubClient client(espClient);

int reading;



void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Messagearrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void setup()
{
    // 初始化HX711
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(CALIBRATION_FACTOR);
    scale.tare();

    // 开启串口
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // 链接wifi
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connectingto WiFi..");
    }
    Serial.println("Connectedto the WiFi network");
    delay(1000);
    // 链接mqtt服务器
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    while (!client.connected())
    {
        Serial.println("Connectingto MQTT...");
        if (client.connect("ESP32Client", mqttUser, mqttPassword))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failedwith state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    delay(1000);

}

void loop()
{
    // Serial.println("进入loop");
    if (scale.wait_ready_timeout(200))
    {
        reading = round(scale.get_units());
        Serial.print(reading);
        itoa(reading * 1000 , message_data, 10);
        Serial.print("HX711 reading: ");
        Serial.println(reading);
    }
    else
    {
        Serial.println("HX711 not found.");
    }

    client.loop();
    client.publish(mqtt_topic, message_data);
    Serial.println(message_data);

    delay(10000);
}
