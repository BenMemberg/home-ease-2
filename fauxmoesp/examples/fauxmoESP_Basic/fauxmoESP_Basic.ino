#include <Arduino.h>
#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"
#include <IRsend.h>    // IR library 
#include <IRrecv.h>
#include <IRutils.h>

// Rename the credentials.sample.h file to credentials.h and 
// edit it according to your router configuration
#include "credentials.h"

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE     115200

// #define LED_YELLOW          4
// #define LED_GREEN           5
// #define LED_BLUE            0
// #define LED_PINK            2
// #define LED_WHITE           15

// #define ID_YELLOW           "yellow lamp"
// #define ID_GREEN            "green lamp"
// #define ID_BLUE             "blue lamp"
// #define ID_PINK             "pink lamp"
// #define ID_WHITE            "white lamp"

#define ID_TV       "tv"
#define ID_MUTE     "mute"
#define ID_VOLUP    "tv volume up"
#define ID_VOLDOWN  "tv volume down"

const uint16_t kRecvPin = 14;
const uint16_t kIrLed = 5;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

int power = 0x0020DF10EF;
int up    = 0x0020DF40BF;
int down  = 0x0020DFC03F;
int mute  = 0x0020DF906F;
int input = 0x0020DFD02F;

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
IRrecv irrecv(kRecvPin);

// -----------------------------------------------------------------------------
//turn on/off the tv by sending IR command
void togglePower();
void muteVolume();
void volumeUp();
void volumeDown();

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void setup() {

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    //initialize the IR 
    IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
    IRrecv irrecv(kRecvPin);
    irsend.begin();
    irrecv.enableIRIn();  // Start the receiver

    // Wifi
    wifiSetup();

    // By default, fauxmoESP creates it's own webserver on the defined port
    // The TCP port must be 80 for gen3 devices (default is 1901)
    // This has to be done before the call to enable()
    fauxmo.createServer(true); // not needed, this is the default value
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
    fauxmo.enable(true);

    // You can use different ways to invoke alexa to modify the devices state:
    // "Alexa, turn yellow lamp on"
    // "Alexa, turn on yellow lamp
    // "Alexa, set yellow lamp to fifty" (50 means 50% of brightness, note, this example does not use this functionality)

    // Add virtual devices
    fauxmo.addDevice(ID_TV     );
    fauxmo.addDevice(ID_MUTE   );
    fauxmo.addDevice(ID_VOLUP  );
    fauxmo.addDevice(ID_VOLDOWN);

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        
        // Callback when a command from Alexa is received. 
        // You can use device_id or device_name to choose the element to perform an action onto (relay, LED,...)
        // State is a boolean (ON/OFF) and value a number from 0 to 255 (if you say "set kitchen light to 50%" you will receive a 128 here).
        // Just remember not to delay too much here, this is a callback, exit as soon as possible.
        // If you have to do something more involved here set a flag and process it in your main loop.
        
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);

        // Checking for device_id is simpler if you are certain about the order they are loaded and it does not change.
        // Otherwise comparing the device_name is safer.

        if (strcmp(device_name, ID_TV)==0) {
            togglePower();
        } else if (strcmp(device_name, ID_MUTE)==0) {
            muteVolume();
        } else if (strcmp(device_name, ID_VOLUP)==0) {
            volumeUp();
        } else if (strcmp(device_name, ID_VOLDOWN)==0) {
            volumeDown();
        }
    });

}

void loop() {

    // fauxmoESP uses an async TCP server but a sync UDP server
    // Therefore, we have to manually poll for UDP packets
    fauxmo.handle();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }

    // If your device state is changed by any other means (MQTT, physical button,...)
    // you can instruct the library to report the new state to Alexa on next request:
    // fauxmo.setState(ID_YELLOW, true, 255);

}

void toggleTv()
{
  debugPrintln("Sending IR command to toggle tv");  
  irsend.sendNEC(power, 32); 
}

void muteVolume()
{
  debugPrintln("Sending IR command to toggle mute"); 
  irsend.sendNEC(mute, 32); 
}

void volumeUp()
{
  debugPrintln("Sending IR command to turn volume up"); 
  irsend.sendNEC(up, 32); 
}

void volumeDown()
{
  debugPrintln("Sending IR command to turn volume down"); 
  irsend.sendNEC(down, 32, 2); 
}

