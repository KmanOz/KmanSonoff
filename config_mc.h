/*
  ======================================================================================================================================
                                            Modify all parameters below to suit you environment
  ======================================================================================================================================
*/
bool rememberRelayState1 = true;                              // If 'true' remembers the state of relay 1 before power loss.
bool rememberRelayState2 = true;                              // If 'true' remembers the state of relay 2 before power loss.
bool rememberRelayState3 = true;                              // If 'true' remembers the state of relay 3 before power loss.
bool rememberRelayState4 = true;                              // If 'true' remembers the state of relay 4 before power loss.
                                                              // Each relay will be OFF evey time power is applied when set to 'false'
                                                              
int kRetain = 0;                                              // Retain mqtt messages (0 for off, 1 for on)
int kUpdFreq = 1;                                             // Update frequency in Mintes to check for mqtt connection. Defualt 1 min.
int kRetries = 10;                                            // WiFi retry count (10 default). Increase if not connecting to your WiFi.
int QOS = 0;                                                  // QOS level for all mqtt messages. (0 or 1)

#define CH_1                                                  // Channel 1 (Default single channel. Do not comment out)
//#define CH_2                                                  // Channel 2 (Uncomment to use 2nd Channel)
//#define CH_3                                                  // Channel 3 (Uncomment to use 3rd Channel)
//#define CH_4                                                  // Channel 4 (Uncomment to use 4th Channel)

#define MQTT_SERVER      "192.168.0.100"                      // Your mqtt server ip address
#define MQTT_PORT        1883                                 // Your mqtt port
#define MQTT_TOPIC       "home/sonoff/living_room/1"          // Base mqtt topic
#define MQTT_USER        "mqtt_user"                          // mqtt username
#define MQTT_PASS        "mqtt_pass"                          // mqtt password

#define WIFI_SSID        "wifissid"                           // Your WiFi ssid
#define WIFI_PASS        "wifipass"                           // Your WiFi password
#define OTA_PASS         0				                            // OTA Password for reflashing - if 0 disabled
/*
  ======================================================================================================================================
*/
