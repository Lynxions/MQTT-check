import paho.mqtt.client as mqtt

#state of connection
def on_connect(client, userdata, flags, rc):
    client_subscriptions(client)
    print("Connected to MQTT server")

# a callback functions
def callback_esp32_sensor1(client, userdata, message):
    print("ESP sensor1 data: ", message.payload.decode('utf-8'))

def callback_esp32_sensor2(client,userdata,message):
    print("ESP sensor2 data: ", message.payload.decode('utf-8'))

def callback_rpi_broadcast(client,userdata,message):
    print("RPi Broadcast message: ", str(message.payload.decode('utf-8')))

def client_subscriptions(client):
    client.subscribe("esp32/#",0)
    client.subscribe("rpi/broadcast",0)

#main
client = mqtt.Client("rpi_client1")

client.on_connect = on_connect

client.message_callback_add('esp32/sensor1', callback_esp32_sensor1)
client.message_callback_add('esp32/sensor2',callback_esp32_sensor2)
client.message_callback_add('rpi/broadcast',callback_rpi_broadcast)

client.connect('',1883,60)
client_subscriptions(client)

client.loop_forever()
