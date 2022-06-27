//#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266WiFi.h>
 
//#ifndef Pins_Arduino_h
#define Pins_Arduino_h

/*
#define PIN_WIRE_SDA (4)
#define PIN_WIRE_SCL (5)
*/
/*
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;
*/

/*
static const uint8_t D0   = 16;
static const uint8_t D1   = 5;
static const uint8_t D2   = 4;
static const uint8_t D3   = 0;
static const uint8_t D4   = 2; LED
static const uint8_t D5   = 14;
static const uint8_t D6   = 12;
static const uint8_t D7   = 13;
static const uint8_t D8   = 15;
static const uint8_t RX   = 3;
static const uint8_t TX   = 1;
*/

/************************* Pin Definition *********************************/

#define Ledd1 14 //RELE
#define Led2 2 //Indicador (PC en 0)

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "#YOUR WIFI NAME"
#define WLAN_PASS       "#YOUR WIFI PASSWORD"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  " #YOUR USERNAME"
#define AIO_KEY       "#YOUR AIOKEY (aiokjasfhgaklsjdhf)"
/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe Led1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Led1");


/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(Ledd1, OUTPUT);
  pinMode(Led2, OUTPUT);


  Serial.println(F("Starting..."));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  delay(500);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  Serial.println("Accesing...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  
  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Led1);

}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Led1) {
      int Led1_State = atoi((char *)Led1.lastread);
      if (Led1_State == 1){
      Serial.print(F("Got: "));
      Serial.println((char *)Led1.lastread);
  digitalWrite(Ledd1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(Led2, HIGH); 
  delay(1000);  
  digitalWrite(Ledd1, LOW);
  digitalWrite(Led2, LOW); 
  delay(100);    
  }
}
}
  
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  
}
// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  digitalWrite(Led2, HIGH);
  delay(200);
  digitalWrite(Led2, LOW);
  delay(200);
  digitalWrite(Led2, HIGH);
  delay(200);
  digitalWrite(Led2, LOW);
  delay(200);
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(Led2, HIGH);
  delay(2000);
  digitalWrite(Led2, LOW);
}
