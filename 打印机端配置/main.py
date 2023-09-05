import sys
import os
import time
import paho.mqtt.client as mqtt

sys.path.append(os.path.abspath(os.path.dirname(__file__) + '/' + '..'))
sys.path.append("..")

REPORT_TOPIC = 'data'  # 主题
file_name = "weight"
server_add = "192.168.6.114"

def on_connect(client, userdata, flags, rc):
    print('connected to mqtt with resurt code ', rc)
    client.subscribe(REPORT_TOPIC)  # 订阅主题


def on_message(client, userdata, msg):
    message = msg.payload.decode()
    file = open(file_name, "w+")
    print(message)
    file.write(message)
    file.close()


def server_conenet(client):
    client.on_connect = on_connect  # 启用订阅模式
    client.on_message = on_message  # 接收消息
    client.connect(server_add , 1883, 60)  # 链接
    # client.loop_start()   # 以start方式运行，需要启动一个守护线程，让服务端运行，否则会随主线程死亡
    client.loop_forever()  # 以forever方式阻塞运行。


def server_stop(client):
    client.loop_stop()  # 停止服务端
    sys.exit(0)


def server_main():
    client_id = time.strftime('%Y%m%d%H%M%S', time.localtime(time.time()))
    client = mqtt.Client(client_id, transport='tcp')
    server_conenet(client)


if __name__ == '__main__':
    # 启动监听
    server_main()
