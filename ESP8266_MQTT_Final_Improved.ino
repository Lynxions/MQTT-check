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

Ticker lockOffTickers[4];

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

const int lockPins[4] = {5, 14, 4, 12}; // ESP32 pins GPIO5 (D1), GPIO14 (D5), GPIO4 (D2), GPIO12 (D6)
const char* lockTopics[4] = {"rpi/locker1", "rpi/locker2", "rpi/locker3", "rpi/locker4"};

bool lockStates[4] = {false, false, false, false};
bool controlLocks[4] = {false, false, false, false};
unsigned long lockOnTimes[4] = {0, 0, 0, 0};

const long interval = 5000; // interval of 5s

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
        blink_led(2, 200); //blink LED twice (for 200ms ON time) to indicate that wifi not connected
        delay(100);
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

        //now attempt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if (client.connect("ESP8266_client1")) { // Change the name of client here if multiple ESP32 are connected
            //attempt successful
            Serial.println("connected");
            // Subscribe to topics here
            for (int i = 0; i < 4; i++) {
                client.subscribe(lockTopics[i]);
            }
        } 
        else {
            //attempt not successful
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 2 seconds");
        
            blink_led(3, 200); //blink LED three times (200ms on duration) to show that MQTT server connection attempt failed
            // Wait 2 seconds before retrying
            delay(2000);
        }    
    }
}

void handleLock(int lockIndex, bool turnOn) {
    if (turnOn) {
        digitalWrite(lockPins[lockIndex], HIGH);
        lockStates[lockIndex] = true;
        lockOnTimes[lockIndex] = millis();
        Serial.print("Lock ");
        Serial.print(lockIndex + 1);
        Serial.println(" is open");
        controlLocks[lockIndex] = true;
    } else {
        digitalWrite(lockPins[lockIndex], LOW);
        lockStates[lockIndex] = false;
        Serial.print("Lock ");
        Serial.print(lockIndex + 1);
        Serial.println(" is closed");
        controlLocks[lockIndex] = false;
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
        if (String(topic) == lockTopics[i]) {
            if (messageTemp == "on" && !lockStates[i]) {
                handleLock(i, true);
            }
            if (messageTemp == "off") {
                handleLock(i, false);
            }
        }
    }
}

void setup(){
    Serial.begin(115200);
        
    for (int i = 0; i < 4; i++) {
        pinMode(lockPins[i], OUTPUT);
    }

    setup_wifi();
    client.setServer(mqtt_server, 1883); //1883 is the default port for MQTT server
    client.setCallback(callback);  
}

void publishLockOff(int lockIndex) {
    client.publish(lockTopics[lockIndex], "off", 0);
}

void topicPublishOn() {
    for (int i = 0; i < 4; i++) {
        if (controlLocks[i]) {
            client.publish(lockTopics[i], "on", 0);
            controlLocks[i] = false;
            lockOffTickers[i].once(5, std::bind(publishLockOff, i));
        }
    }
}

void loop(){
    if (!client.connected()){
        connect_mqttServer();
    }
    client.loop();

    //function to publish
    unsigned long currentMillis = millis();
    for (int i = 0; i < 4; i++) {
        if (lockStates[i] && currentMillis - lockOnTimes[i] >= interval) {
            digitalWrite(lockPins[i], LOW);
            lockStates[i] = false;
            Serial.print("Lock ");
            Serial.print(i + 1);
            Serial.println(" is closed after 5 seconds");
        }
    }

    topicPublishOn();
}
