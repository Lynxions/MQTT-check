#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

// const char* ssid = "IU STAFF";
// const char* password = "8888888888";

const char* ssid = "International University";
const char* password = "";

//const char* mqtt_server = "broker.hivemq.com";

const char* mqtt_server = "10.238.55.140"; //IU

//const char* mqtt_server = "10.8.99.120"; //IU STAFF

#define ledPin 2

Ticker publishOffTicker;

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lockOnTime[] = {0, 0, 0, 0};
bool control_lock[] = {false, false, false, false};
bool lockState[] = {false, false, false, false};
const byte lock_pin[] = {5, 14, 4, 12}; // D1, D5, D2, D6

const long interval = 5000; // interval of 5s

//const byte button = 0; //ESP32 pin GPIO0 (D3)
// int lock_state = LOW;    // the current state of Lock
// //int button_state;       // the current state of button
// int last_button_state;  // the previous state of button

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

    for (int i = 0; i < 4; i++) {
        String lockerTopic = "rpi/locker" + String(i + 1);
        if (String(topic) == lockerTopic) {
            handleLockMessage(i, messageTemp);
            break;
        }
    }
}

void handleLockMessage(int lockIndex, String messageTemp) {
    if (messageTemp == "on" && !lockState[lockIndex]) {
        digitalWrite(lock_pin[lockIndex], HIGH);
        lockState[lockIndex] = true;
        lockOnTime[lockIndex] = millis(); // store the time when lock is turned on
        Serial.println("Lock " + String(lockIndex + 1) + " is open");
        control_lock[lockIndex] = true;
    }
    if (messageTemp == "off") {
        digitalWrite(lock_pin[lockIndex], LOW);
        lockState[lockIndex] = false;
        Serial.println("Lock " + String(lockIndex + 1) + " is closed");
        control_lock[lockIndex] = false;
    }
}

void setup(){
    Serial.begin(115200);
        
    for (int i = 0; i < 4; i++) {
        pinMode(lock_pin[i], OUTPUT);
    }

//    pinMode(button, INPUT_PULLUP);

//    button_state = digitalRead(button);


    setup_wifi();
    client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
    client.setCallback(callback);  

}

void pubOffTicker(){
  publishOffTicker.once(5, topicPublishOff); // Set timer for 5 seconds to publish 'off' payload 
}

void topicPublishOff(){
    char f[] = "off";
    for (int i = 0; i < 4; i++) {
        if (!control_lock[i]) {
            String topic = "rpi/locker" + String(i + 1);
            client.publish(topic.c_str(), f, 0);
        }
    }
}
void topicPublishOn(){
    char o[] = "on";
    for (int i = 0; i < 4; i++) {
        if (control_lock[i]) {
            String topic = "rpi/locker" + String(i + 1);
            client.publish(topic.c_str(), o, 0);
            control_lock[i] = false;  
            pubOffTicker(); 
        }
    }
}


void loop(){
    if (!client.connected()){
        connect_mqttServer();
    }
    client.loop();

    //function to publish
    // Function to close locks after interval
    unsigned long currentMillis = millis();
    for (int i = 0; i < 4; i++) {
        if (lockState[i] && currentMillis - lockOnTime[i] >= interval) {
            digitalWrite(lock_pin[i], LOW);
            lockState[i] = false;
            Serial.println("Lock " + String(i + 1) + " is closed after 5 seconds");
        }
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
