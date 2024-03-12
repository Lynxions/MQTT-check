import time
import paho.mqtt.client as mqtt
from flask import Flask, render_template, request

app = Flask(__name__)

def on_publish(client, userdata, mid):
    print("message published")

@app.route("/arduino/cell1/on")
def cell1_on():
    msg = "on"
    pubMsg = client_pub.publish(
        topic='rpi/broadcast',
        payload=msg.encode('utf-8'),
        qos=0,
    )
    pubMsg.wait_for_publish()

@app.route("/arduino/cell1/off")
def cell1_off():
    msg = "off"
    pubMsg = client_pub.publish(
        topic='rpi/broadcast',
        payload=msg.encode('utf-8'),
        qos=0,
    )
    pubMsg.wait_for_publish() 

if __name__ == "__main__":
    app.run(debug=True)

client_pub = mqtt.Client("rpi_client_pub") #this name should be unique
client_pub.on_publish = on_publish
client_pub.connect('10.238.35.127',1883,60)
# start a new thread
client_pub.loop_start()


k=0
while True:
    k=k+1
    if(k>20):
        k=1 
        
    try:
        msg =str(k)
        pubMsg = client_pub.publish(
            topic='rpi/broadcast',
            payload=msg.encode('utf-8'),
            qos=0,
        )
        pubMsg.wait_for_publish()
        print(pubMsg.is_published())
    
    except Exception as e:
        print(e)
        
    time.sleep(2)
