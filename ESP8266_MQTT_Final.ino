#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

// const char* ssid = "IU STAFF";
// const char* password = "8888888888";

const char* ssid = "International University";
const char* password = "";

const char* mqtt_server = "broker.hivemq.com";

//const char* mqtt_server = "10.238.55.140"; //IU

//const char* mqtt_server = "10.8.99.120"; //IU STAFF

#define ledPin 2

Ticker lock1Ticker;
Ticker lock2Ticker;
Ticker lock3Ticker;
Ticker lock4Ticker;

Ticker lock1OffTicker;
Ticker lock2OffTicker;
Ticker lock3OffTicker;
Ticker lock4OffTicker;

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lock1OnTime = 0;
unsigned long lock2OnTime = 0;
unsigned long lock3OnTime = 0;
unsigned long lock4OnTime = 0;
const long interval = 5000; // interval of 5s

//const byte button = 0; //ESP32 pin GPIO0 (D3)
const byte lock_pin1 = 5; //ESP32 pin GPIO5 (D1)
const byte lock_pin2 = 14; //ESP32 pin GPIO3 (D5)
const byte lock_pin3 = 4; //ESP32 pin GPIO4 (D2)
const byte lock_pin4 = 12; //ESP32 pin GPIO13 (D6)

bool control_lock1 = false;
bool control_lock2 = false;
bool control_lock3 = false;
bool control_lock4 = false;

// int lock_state = LOW;    // the current state of Lock
// //int button_state;       // the current state of button
// int last_button_state;  // the previous state of button

bool lockState1 = false;
bool lockState2 = false;
bool lockState3 = false;
bool lockState4 = false;

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
            client.subscribe("rpi/locker3");
            client.subscribe("rpi/locker4");
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
      if (messageTemp == "on" && !lockState1) {
        digitalWrite(lock_pin1, HIGH);
        lockState1 = true;
        lock1OnTime = millis(); // store the time when lock 1 is turned on
        Serial.println("Lock 1 is open");
        control_lock1 = true;
      }
      if (messageTemp == "off") {
        digitalWrite(lock_pin1, LOW);
        lockState1 = false;
        Serial.println("Lock 1 is closed");
        control_lock1 = false;
      }
    }

    if (String(topic) == "rpi/locker2") {
      if (messageTemp == "on" && !lockState2) {
        digitalWrite(lock_pin2, HIGH);
        lockState2 = true;
        lock2OnTime = millis(); // store the time when lock 2 is turned on
        Serial.println("Lock 2 is open");
        control_lock2 = true;
      }
      if (messageTemp == "off") {
        digitalWrite(lock_pin2, LOW);
        lockState2 = false;
        Serial.println("Lock 2 is closed");
        control_lock2 = false;
      }
    }

    if (String(topic) == "rpi/locker3") {
      if (messageTemp == "on" && !lockState3) {
        digitalWrite(lock_pin3, HIGH);
        lockState3 = true;
        lock3OnTime = millis(); // store the time when lock 3 is turned on
        Serial.println("Lock 3 is open");
        control_lock3 = true;
      }
      if (messageTemp == "off") {
        digitalWrite(lock_pin3, LOW);
        lockState3 = false;
        Serial.println("Lock 3 is closed");
        control_lock3 = false;
      }
    }

    if (String(topic) == "rpi/locker4") {
      if (messageTemp == "on" && !lockState4) {
        digitalWrite(lock_pin4, HIGH);
        lockState4 = true;
        lock4OnTime = millis(); // store the time when lock 4 is turned on
        Serial.println("Lock 4 is open");
        control_lock4 = true;
      }
      if (messageTemp == "off") {
        digitalWrite(lock_pin4, LOW);
        lockState4 = false;
        Serial.println("Lock 4 is closed");
        control_lock4 = false;
      }
    }
}

void setup(){
    Serial.begin(115200);
        
    pinMode(lock_pin1, OUTPUT);
    pinMode(lock_pin2, OUTPUT);
    pinMode(lock_pin3, OUTPUT);
    pinMode(lock_pin4, OUTPUT);

//    pinMode(button, INPUT_PULLUP);

//    button_state = digitalRead(button);


    setup_wifi();
    client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
    client.setCallback(callback);  

}

void lock1PublishOff() {
  char a[] = "rpi/locker1";
  char f[] = "off";
  client.publish(a, f, 0);
}

void lock2PublishOff() {
  char b[] = "rpi/locker2";
  char f[] = "off";
  client.publish(b, f, 0);
}

void lock3PublishOff() {
  char c[] = "rpi/locker3";
  char f[] = "off";
  client.publish(c, f, 0);
}

void lock4PublishOff() {
  char d[] = "rpi/locker4";
  char f[] = "off";
  client.publish(d, f, 0);
}

void topicPublishOn(){
  char o[] = "on";
  char a[] = "rpi/locker1";
  char b[] = "rpi/locker2";
  char c[] = "rpi/locker3";
  char d[] = "rpi/locker4";

  if (control_lock1) {
    client.publish(a, o, 0);
    control_lock1 = false;
    lock1OffTicker.once(5, lock1PublishOff); 
  }

  if (control_lock2) {
    client.publish(b, o, 0);
    control_lock2 = false;
    lock2OffTicker.once(5, lock2PublishOff); 
  }

  if (control_lock3) {
    client.publish(c, o, 0);
    control_lock3 = false;
    lock3OffTicker.once(5, lock3PublishOff); 
  }

  if (control_lock4) {
    client.publish(d, o, 0);
    control_lock4 = false;
    lock4OffTicker.once(5, lock4PublishOff); 
  }
}

void loop(){
    if (!client.connected()){
        connect_mqttServer();
    }
    client.loop();

    //function to publish
    unsigned long currentMillis = millis();
    if (lockState1 && currentMillis - lock1OnTime >= interval) {
        digitalWrite(lock_pin1, LOW);
        lockState1 = false;
        Serial.println("Lock 1 is closed after 5 seconds");
    }

    if (lockState2 && currentMillis - lock2OnTime >= interval) {
        digitalWrite(lock_pin2, LOW);
        lockState2 = false;
        Serial.println("Lock 2 is closed after 5 seconds");
    }

    if (lockState3 && currentMillis - lock3OnTime >= interval) {
        digitalWrite(lock_pin3, LOW);
        lockState3 = false;
        Serial.println("Lock 3 is closed after 5 seconds");
    }

    if (lockState4 && currentMillis - lock4OnTime >= interval) {
        digitalWrite(lock_pin4, LOW);
        lockState4 = false;
        Serial.println("Lock 4 is closed after 5 seconds");
    }

    // button
    // last_button_state = button_state;
    // button_state = digitalRead(button);

    topicPublishOn();

    // if(last_button_state == HIGH && button_state == LOW){
    //   lock_state = !lock_state;
    //   digitalWrite(lock_pin, lock_state);
    // }
    //-------------
}
