/*
  ===========================================================================================================================================
                                            Modify all parameters below to suit you environment
  ===========================================================================================================================================
*/
bool rememberRelayState = true;                               // If 'true' remembers the state of the relay before power loss otherwise
                                                              // load will be OFF evey time power is applied. Set retain below to 0 if true.
                                                              
int kRetain = 0;                                              // Retain mqtt messages (0 for off, 1 for on)
int kUpdFreq = 1;                                             // Update frequency in Mintes to check for mqtt connection. Defualt 1 min.
int kRetries = 10;                                            // WiFi retry count (10 default). Increase if not connecting to your WiFi.
int QOS = 0;                                                  // QOS level for all mqtt messages. (0 or 1)

#define NONE                                                  // Set to NONE, TEMP, or WS (Cannot be blank)
                                                              // NONE for standard Sonoff relay only ON / OFF (default)
                                                              // TEMP for DHT11/22 Support on Pin 5 of header (GPIO 14)  **Must install 'DHT sensor library' (Adafruit) & 'Adafruit Unified Sensor' library.
                                                              // WS for External Wallswitch Support on Pin 5 of header (GPIO 14)

#define ORIG                                                  // ORIG or TH
                                                              // ORIG for Basic / Original Sonoff, TH for TH Series

#define DHTTYPE          DHT22                                // Set to 'DHT11' or 'DHT22'. (Only applies if using TEMP)  **Must connect to the mains power for temperature readings to be sent.
#define UseFahrenheit    false                                // Set to 'true' to use Fahrenheit. (Only applies if using TEMP)

#define MQTT_SERVER      "192.168.0.100"                      // Your mqtt server ip address
#define MQTT_PORT        1883                                 // Your mqtt port
#define MQTT_TOPIC       "home/sonoff/living_room/1"          // Base mqtt topic
#define MQTT_USER        "mqtt_user"                          // mqtt username
#define MQTT_PASS        "mqtt_pass"                          // mqtt password

#define WIFI_SSID        "wifissid"                           // Your WiFi ssid
#define WIFI_PASS        "wifipass"                           // Your WiFi password
/*
  ===========================================================================================================================================
*/
