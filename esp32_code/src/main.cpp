#include "HX711.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <conf.h>
// #include <conf_myself.h>



HX711 scale;

#if(setCalibration == false) //关闭setCalibration为普通模式测量
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
        // 开启串口
        Serial.begin(115200);
        // 启动EEPROM
        EEPROM.begin(1024);
        pinMode(LED1_HX711,OUTPUT);
        pinMode(LED2_WIFI,OUTPUT);
        pinMode(LED3,OUTPUT);
        //判断是否为空
        if (EEPROM.read(10) == 0){
            EEPROM.write(10,(uint8_t)500);
        }

        //开始初始化HX711，以亮灯为正在初始化
        digitalWrite(LED1_HX711,HIGH);
        // 初始化HX711
        scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
        scale.set_scale(CALIBRATION_FACTOR);
        scale.tare();
        scale.set_offset(EEPROM.read(10)); //称重偏移
        digitalWrite(LED1_HX711,LOW);
        //初始化成功LED1关闭

        //开始初始化WIFI并并且点亮LED2
        digitalWrite(LED2_WIFI,HIGH);
        WiFi.begin(ssid, password);

        // 链接wifi
        Serial.println("Connectingto WiFi..");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print("...");
        }
        Serial.println("Connectedto the WiFi network");

        //WIFI链接成功关闭LED2
        digitalWrite(LED2_WIFI,LOW);

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
        //全部连接成功点亮LED3表明初始化结束
        digitalWrite(LED3,HIGH);
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
        EEPROM.write(10,reading * 1000);

        delay(10000);
    }

#elif(setCalibration == true) //开启setCalibration为进行去皮计算

    void setup() {
    Serial.begin(115200);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    }

    void loop() {

    if (scale.is_ready()) {
        scale.set_scale();
        Serial.println("Tare... remove any weights from the scale.");
        delay(5000);
        scale.tare();
        Serial.println("Tare done...");
        Serial.print("Place a known weight on the scale...");
        delay(5000);
        long reading = scale.get_units(10);
        Serial.print("Result: ");
        Serial.println(reading);
    }
    else {
        Serial.println("HX711 not found.");
    }
    delay(1000);
    }

    //calibration factor will be the (reading)/(known weight)

#endif
