#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <Ticker.h>

// const char* ssid = "IU STAFF";
// const char* password = "8888888888";

const char* ssid = "International University";
const char* password = "";

//const char* mqtt_server = "broker.hivemq.com";

const char* mqtt_server = "10.238.55.140"; //IU

//const char* mqtt_server = "10.8.99.120"; //IU STAFF

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

#define ledPin 2

// Ticker lockStateTick;
// Ticker lockStateTick2;

unsigned long prevMilli = 0; // store the last time lockstate was updated
const long interval = 5000; // interval of 5s

const byte lock_pin = 5; //ESP32 pin GPIO5 (D1)
const byte control_pin = 16; //ESP32 pin GPIO16 (D0)
//const byte button = 0; //ESP32 pin GPIO0 (D3)
const byte lock_pin2 = 14; //ESP32 pin GPIO3 (D5)

int lock_state = LOW;    // the current state of Lock
//int button_state;       // the current state of button
int last_button_state;  // the previous state of button

bool lockState = false;
bool lockState2 = false;

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
        delay(100); //
        Serial.print(".");
        c=c+1;
        if(c>50){
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
            client.subscribe("rpi/locker1");
            client.subscribe("rpi/locker2");
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
        messageTemp += (char)message[i];
    }

    if (String(topic) == "rpi/locker1") {
      if (messageTemp == "on" && !lockState) {
        digitalWrite(lock_pin, HIGH);
        lockState = true;
        Serial.print("Lock 1 is open");

        Serial.println();
        delay(5000);
        digitalWrite(lock_pin, LOW);
        lockState = false;
        Serial.print("Lock 1 is closed after 5 seconds");
      }
      if (messageTemp == "off") {
        digitalWrite(lock_pin, LOW);
        lockState = false;
        Serial.print("Lock 1 is closed");
      }
    }
    Serial.println();

    if (String(topic) == "rpi/locker2") {
      if (messageTemp == "on" && !lockState2) {
        digitalWrite(lock_pin2, HIGH);
        lockState2 = true;
        Serial.print("Lock 2 is open");

        Serial.println();
        delay(5000);
        digitalWrite(lock_pin2, LOW);
        lockState2 = false;
        Serial.print("Lock 2 is closed after 5 seconds");
      }
      if (messageTemp == "off") {
        digitalWrite(lock_pin2, LOW);
        lockState2 = false;
        Serial.print("Lock 2 is closed");
      }
    }
    Serial.println();
}


void setup(){
    Serial.begin(115200);
    
    pinMode(control_pin, INPUT);
    pinMode(lock_pin, OUTPUT);
//    pinMode(button, INPUT_PULLUP);
    pinMode(lock_pin2, OUTPUT);

//    button_state = digitalRead(button);

    setup_wifi();
    client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
    client.setCallback(callback);  

}

void loop(){
    if (!client.connected()){
        connect_mqttServer();
    }

    client.loop();

    //function to publish

    unsigned long currentMilli = millis();

    int analogReading = analogRead(control_pin);
    
    // button
    // last_button_state = button_state;
    // button_state = digitalRead(button);

    
    // if(last_button_state == HIGH && button_state == LOW){
    //   lock_state = !lock_state;
    //   digitalWrite(lock_pin, lock_state);
    // }
    //-------------

    if(analogReading == 0) {
      digitalWrite(lock_pin, LOW);
      
      char* a = "off";
      char* b = "rpi/locker1";
      client.publish(b, a, 0);
      char* c = "rpi/locker2";
      client.publish(c, a, 0);
    }
    if(analogReading == 1) {
      digitalWrite(lock_pin, HIGH);
      
      char* a = "on";
      char* o = "off";
      char* b = "rpi/locker1";
      client.publish(b, a, 0);
      char* c = "rpi/locker2";
      client.publish(c, a, 0);
      delay(5000);
      client.publish(b, o, 0);
      client.publish(c, o, 0);
    }
    

}
