/*
  Copyright (c) 2017 @KmanOz
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  *** USE THIS Firmware for: Original Sonoff, Sonoff SV, Sonoff Touch, Sonoff S20 Smart Socket, Sonof TH Series ***

  ================================================================================================
     ATTENTION !!!!!! DO NOT CHANGE ANYTHING IN THIS SECTION UNLESS YOU KNOW WHAT YOU ARE DOING
  ================================================================================================
*/

#include "config_sc.h"
#ifdef TEMP
#include "DHT.h"
#endif
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <Ticker.h>

#define B_1           0
#define L_1           12
#define LED           13
#if defined (TEMP) || defined (WS)
#define OPT_PIN       14
#endif
#define HOST_PREFIX   "Sonoff_%s"
#define HEADER        "\n\n---------------------  KmanSonoff_v1.00sc  -------------------"
#define VER           "kssc_v1.00"

bool OTAupdate = false;
bool sendStatus = false;
bool requestRestart = false;
bool tempReport = false;
char ESP_CHIP_ID[8];
char UID[16];
#ifdef WS
int wallSwitch = 1;
int lastWallSwitch = 1;
#endif
int lastRelayState;
long rssi;
unsigned long TTasks1;
unsigned long count = 0;
#ifdef TEMP
DHT dht(OPT_PIN, DHTTYPE, 11);
#endif
extern "C" { 
  #include "user_interface.h" 
}
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient, MQTT_SERVER, MQTT_PORT);
Ticker btn_timer;

void callback(const MQTT::Publish& pub) {
  if (pub.payload_string() == "stat") {
  }
  else if (pub.payload_string() == "on") {
    #ifdef ORIG
    digitalWrite(LED, LOW);
    #endif
    digitalWrite(L_1, HIGH);
  }
  else if (pub.payload_string() == "off") {
    #ifdef ORIG
    digitalWrite(LED, HIGH);
    #endif
    digitalWrite(L_1, LOW);
  }
  else if (pub.payload_string() == "reset") {
    requestRestart = true;
  }
  sendStatus = true;
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(L_1, OUTPUT);
  pinMode(B_1, INPUT);
  #ifdef WS
  pinMode(OPT_PIN, INPUT_PULLUP);
  #endif
  digitalWrite(LED, HIGH);
  digitalWrite(L_1, LOW);
  Serial.begin(115200);
  sprintf(ESP_CHIP_ID, "%06X", ESP.getChipId());
  sprintf(UID, HOST_PREFIX, ESP_CHIP_ID);
  EEPROM.begin(8);
  lastRelayState = EEPROM.read(0);
  if (rememberRelayState && lastRelayState == 1) {
    #ifdef ORIG
    digitalWrite(LED, LOW);
    #endif
    digitalWrite(L_1, HIGH);
  }
  btn_timer.attach(0.05, button);
  mqttClient.set_callback(callback);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(UID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  ArduinoOTA.setHostname(UID);
  ArduinoOTA.onStart([]() {
    OTAupdate = true;
    blinkLED(LED, 400, 2);
    digitalWrite(LED, HIGH);
    Serial.println("OTA Update Initiated . . .");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA Update Ended . . .s");
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    digitalWrite(LED, LOW);
    delay(5);
    digitalWrite(LED, HIGH);
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    blinkLED(LED, 40, 2);
    OTAupdate = false;
    Serial.printf("OTA Error [%u] ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(". . . . . . . . . . . . . . . Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println(". . . . . . . . . . . . . . . Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println(". . . . . . . . . . . . . . . Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println(". . . . . . . . . . . . . . . Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println(". . . . . . . . . . . . . . . End Failed");
  });
  ArduinoOTA.begin();
  Serial.println(HEADER);
  Serial.print("\nUID: ");
  Serial.print(UID);
  Serial.print("\nConnecting to "); Serial.print(WIFI_SSID); Serial.print(" Wifi"); 
  while ((WiFi.status() != WL_CONNECTED) && kRetries --) {
    delay(500);
    Serial.print(" .");
  }
  if (WiFi.status() == WL_CONNECTED) {  
    Serial.println(" DONE");
    Serial.print("IP Address is: "); Serial.println(WiFi.localIP());
    Serial.print("Connecting to ");Serial.print(MQTT_SERVER);Serial.print(" Broker . .");
    delay(500);
    while (!mqttClient.connect(MQTT::Connect(UID).set_keepalive(90).set_auth(MQTT_USER, MQTT_PASS)) && kRetries --) {
      Serial.print(" .");
      delay(1000);
    }
    if(mqttClient.connected()) {
      Serial.println(" DONE");
      Serial.println("\n----------------------------  Logs  ----------------------------");
      Serial.println();
      mqttClient.subscribe(MQTT_TOPIC);
      blinkLED(LED, 40, 8);
      #ifdef ORIG
      if(digitalRead(L_1) == HIGH)  {
        digitalWrite(LED, LOW);
      } else {
        digitalWrite(LED, HIGH);
      }
      #endif
      #ifdef TH
      digitalWrite(LED, LOW);
      #endif
    }
    else {
      Serial.println(" FAILED!");
      Serial.println("\n----------------------------------------------------------------");
      Serial.println();
    }
  }
  else {
    Serial.println(" WiFi FAILED!");
    Serial.println("\n----------------------------------------------------------------");
    Serial.println();
  }
}

void loop() {
  ArduinoOTA.handle();
  if (OTAupdate == false) { 
    mqttClient.loop();
    timedTasks1();
    checkStatus();
    #ifdef TEMP
    if (tempReport) {
      getTemp();
    }
    #endif
    #ifdef WS
    checkWallSwitch();
    #endif
  }
}

void blinkLED(int pin, int duration, int n) {             
  for(int i=0; i<n; i++)  {  
    digitalWrite(pin, HIGH);        
    delay(duration);
    digitalWrite(pin, LOW);
    delay(duration);
  }
}

void button() {
  if (!digitalRead(B_1)) {
    count++;
  } 
  else {
    if (count > 1 && count <= 40) {   
      #ifdef ORIG
      digitalWrite(LED, !digitalRead(LED));
      #endif
      digitalWrite(L_1, !digitalRead(L_1));
      sendStatus = true;
    } 
    else if (count >40){
      Serial.println("\n\nSonoff Rebooting . . . . . . . . Please Wait"); 
      requestRestart = true;
    } 
    count=0;
  }
}

void checkConnection() {
  if (WiFi.status() == WL_CONNECTED)  {
    if (mqttClient.connected()) {
      Serial.println("mqtt broker connection . . . . . . . . . . OK");
    } 
    else {
      Serial.println("mqtt broker connection . . . . . . . . . . LOST");
      requestRestart = true;
    }
  }
  else { 
    Serial.println("WiFi connection . . . . . . . . . . LOST");
    requestRestart = true;
  }
}

void checkStatus() {
  if (sendStatus) {
    #ifdef ORIG
    if(digitalRead(LED) == LOW)  {
      if (rememberRelayState) {
        EEPROM.write(0, 1);
      }      
      if (kRetain == 0) {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "on").set_qos(QOS));
      } else {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "on").set_retain().set_qos(QOS));
      }
      Serial.println("Relay . . . . . . . . . . . . . . . . . . ON");
    } else {
       if (rememberRelayState) {
        EEPROM.write(0, 0);
      }       
      if (kRetain == 0) {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "off").set_qos(QOS));
      } else {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "off").set_retain().set_qos(QOS));
      }
      Serial.println("Relay . . . . . . . . . . . . . . . . . . OFF");
    }
    #endif
    #ifdef TH
    if(digitalRead(L_1) == LOW)  {
      if (rememberRelayState) {
        EEPROM.write(0, 0);
      }
      if (kRetain == 0) {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "off").set_qos(QOS));
      } else {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "off").set_retain().set_qos(QOS));
      }
      Serial.println("Relay . . . . . . . . . . . . . . . . . . OFF");
    } else {
      if (rememberRelayState) {
        EEPROM.write(0, 1);
      }
      if (kRetain == 0) {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "on").set_qos(QOS));
      } else {
        mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/stat", "on").set_retain().set_qos(QOS));
      }
      Serial.println("Relay . . . . . . . . . . . . . . . . . . ON");
    }
    #endif
    if (rememberRelayState) {
      EEPROM.commit();
    }    
    sendStatus = false;
  }
  if (requestRestart) {
    blinkLED(LED, 400, 4);
    ESP.restart();
  }
}

#ifdef WS
void checkWallSwitch() {
  wallSwitch = digitalRead(OPT_PIN);
  if (wallSwitch != lastWallSwitch) {
    digitalWrite(L_1, !digitalRead(L_1));
    digitalWrite(LED, !digitalRead(LED));
    sendStatus = true;
  }
  lastWallSwitch = wallSwitch;
}
#endif

#ifdef TEMP
void getTemp() {
  Serial.print("DHT read . . . . . . . . . . . . . . . . . ");
  float dhtH, dhtT;
  char message_buff[60];
  dhtH = dht.readHumidity();
  dhtT = dht.readTemperature();
  if(digitalRead(LED) == LOW)  {
    blinkLED(LED, 100, 1);
  } else {
    blinkLED(LED, 100, 1);
    digitalWrite(LED, HIGH);
  }
  if (isnan(dhtH) || isnan(dhtT)) {
    if (kRetain == 0) {
      mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/debug","\"DHT Read Error\"").set_qos(QOS));
    } else {
      mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/debug","\"DHT Read Error\"").set_retain().set_qos(QOS));
    }
    Serial.println("ERROR");
    tempReport = false;
    return;
  }
  String pubString = "{\"Temp\": "+String(dhtT)+", "+"\"Humidity\": "+String(dhtH) + "}";
  pubString.toCharArray(message_buff, pubString.length()+1);
  if (kRetain == 0) {
    mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/temp", message_buff).set_qos(QOS));
  } else {
    mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/temp", message_buff).set_retain().set_qos(QOS));
  }
  Serial.println("OK");
  tempReport = false;
}
#endif

void doReport() {
  rssi = WiFi.RSSI();
  char message_buff[120];
  String pubString = "{\"UID\": "+String(UID)+", "+"\"WiFi RSSI\": "+String(rssi)+"dBM"+", "+"\"Topic\": "+String(MQTT_TOPIC)+", "+"\"Ver\": "+String(VER)+"}";
  pubString.toCharArray(message_buff, pubString.length()+1);
  if (kRetain == 0) {
    mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/debug", message_buff).set_qos(QOS));
    mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/heartbeat", "OK").set_qos(QOS));
  } else {
    mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/debug", message_buff).set_retain().set_qos(QOS));
    mqttClient.publish(MQTT::Publish(MQTT_TOPIC"/heartbeat", "OK").set_retain().set_qos(QOS));
  }
}

void timedTasks1() {
  if ((millis() > TTasks1 + (kUpdFreq*60000)) || (millis() < TTasks1)) { 
    TTasks1 = millis();
    checkConnection();
    doReport();
    #ifdef TEMP
    tempReport = true;
    #endif
  }
}

