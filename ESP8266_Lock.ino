#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace the SSID/Password details as per your wifi router
const char* ssid = "";
const char* password = "";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
// Replace your MQTT Broker IP address here:
const char* mqtt_server = "";

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

#define ledPin 2
#define LOCK_PIN 16 // ESP32 pin GPIO16 (D0): for on and off switch
#define LOAD_PIN 5 

int ledTestPin = 8;
int relayPin = 7;

void blink_led(unsigned int times, unsigned int duration){
    for (int i = 0; i < times; i++) {
        digitalWrite(ledPin, HIGH);
        delay(duration);
        digitalWrite(ledPin, LOW); 
        delay(200);
    }
}

void setup_wifi(){
    delay(50);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);   

    int c=0;
    while(WiFi.status() != WL_CONNECTED){
        blink_led(2,200); //blink LED twice (for 200ms ON time) to indicate that wifi not connected
        delay(1000); //
        Serial.print(".");
        c++;
        if(c>10){
            ESP.restart(); //restart ESP after 10 seconds
        }        
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void connect_mqttServer(){
    // Loop until we're reconnected
    while (!client.connected()) {

        //first check if connected to wifi
        if(WiFi.status() != WL_CONNECTED){
            //if not connected, then first connect to wifi
            setup_wifi();
        }

        //now attemt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if (client.connect("ESP8266_client1")) { // Change the name of client here if multiple ESP32 are connected
            //attempt successful
            Serial.println("connected");
            // Subscribe to topics here
            client.subscribe("rpi/cell1");
            client.subscribe("rpi/broadcast");
            //client.subscribe("rpi/xyz"); //subscribe more topics here
        } 
        else {
            //attempt not successful
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 2 seconds");
        
            blink_led(3,200); //blink LED three times (200ms on duration) to show that MQTT server connection attempt failed
            // Wait 2 seconds before retrying
            delay(2000);
        }    
    }
}

//this function will be executed whenever there is data available on subscribed topics
void callback(char* topic, byte* message, unsigned int length) {
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    
    for (int i = 0; i < length; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
Serial.println();

    // Check if a message is received on the topic "rpi/broadcast"
    
    
    // if (String(topic) == "rpi/broadcast") {
    //     if(messageTemp == "on"){
    //         Serial.println("Action: blink LED");
    //         blink_led(1,1250); //blink LED once (for 1250ms ON time)
    //     }
    //     else if (messageTemp == "off")
    //     {   
    //         Serial.println("Action: LED off");
    //     }
        
    // }
    

    //Similarly add more if statements to check for other subscribed topics 
}



void setup(){
    Serial.begin(115200);

    setup_wifi();
    client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
    client.setCallback(callback);    
}

void loop(){
    if (!client.connected()){
        connect_mqttServer();
    }

    client.loop();
    /*
    if (!client.loop()){
        client.connect("ESP8266_Client1");
    }
    */

    //function to publish

    int analogReading = analogRead(LOAD_PIN);
    Serial.print("The Sensor is: ");

    if(analogReading == 0) {
      Serial.println("off");
      char* a = "off";
      char* b = "rpi/broadcast";
      client.publish(b, a, 0);
    } else {
      Serial.println("on");
      char* a = "on";
      char* b = "rpi/broadcast";
      client.publish(b, a, 0);
    }
    
    delay(2000);
}
