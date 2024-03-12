import paho.mqtt.client as mqtt
from firebase import firebase

url='https://parcellocker-db892-default-rtdb.asia-southeast1.firebasedatabase.app/'

firebase = firebase.FirebaseApplication(url, None)

val=''
val2=''
#state of connection
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client_subscriptions(client)

def on_publish(client, userdata, message):
    print("message published")

def on_message(client, userdata, message):
    print("Received message '" + str(message.payload) + "' on topic '" + message.topic)

def callback_esp32_sensor1(client, userdata, message):
    #print("ESP arduino1 data: ", message.payload.decode('utf-8'))
    update_database(message)

def callback_esp32_sensor2(client,userdata,message):
    #print("ESP arduino2 data: ", message.payload.decode('utf-8'))
    update_database(message)

def client_subscriptions(client):
    client.subscribe("esp8266/#/#",0)

def update_database(message):
    door = 0
    weight = 0
    if message.topic == "/esp8266/#/door":
        print("door update")
        door = str(message.payload, 'UTF-8')
        door = door.strip()
        print(door)
        global val
        val = door
        
    if message.topic == "/esp8266/#/weight":
        print("weight update")
        weight = str(message.payload, 'UTF-8')
        weight = weight.strip()
        print(weight)
        global val2
        val2 = weight
    
    if val != '' and val2 != '':    
        print(val, val2)
        data = {"door": val, "weight": val2}
        #firebase.put('/esp8266/#/status', data)
        firebase.put("esp8266/arduino#","door", val)
        firebase.put("esp8266/arduino#","weight", val2)
        val = ''
        val2 = ''    

def main():
    client_sub = mqtt.Client("rpi_client_sub")
    client_pub = mqtt.Client("rpi_client_pub")

    client_sub.on_connect = on_connect
    client_sub.on_message = on_message

    client_pub.on_publish = on_publish

    client_sub.message_callback_add('esp8266/arduino1/#', callback_esp32_sensor1)
    client_sub.message_callback_add('esp8266/arduino2/#',callback_esp32_sensor2)    
    
    client_sub.connect('localhost', 1883, 60) 
    client_subscriptions(client)
    client_sub.loop_start()

    client_pub.loop_start()

if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()

k=0
while True:
    k=k+1
    if(k>20):
        k=1 
        
    try:
        msg =str(k)
        pubMsg = client.publish(
            topic='rpi/broadcast',
            payload=msg.encode('utf-8'),
            qos=0,
        )
        pubMsg.wait_for_publish()
        print(pubMsg.is_published())
    
    except Exception as e:
        print(e)
        
    time.sleep(2)
