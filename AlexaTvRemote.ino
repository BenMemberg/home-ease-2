
#include "debug.h"              // Serial debugger printing
#include "WifiConnection.h"     // Wifi connection 
#include "Wemulator.h"
#include "WemoCallbackHandler.h"
#include <IRsend.h>    // IR library 
#include <IRrecv.h>
#include <IRutils.h>

const uint16_t kRecvPin = 14;
const uint16_t kIrLed = 5;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
int power=0x0020DF10EF;
int up=0x0020DF40BF;
int down=0x0020DFC03F;
int mute=0x0020DF906F;
int input=0x0020DFD02F;


WifiConnection* wifi;           // wifi connection
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
IRrecv irrecv(kRecvPin);
Wemulator* wemulator;           // wemo emulator

//This is used as a crude workaround for a threading issue
bool Power = false;   // command flag
bool Mute = false;   // command flag
bool Up = false;   // command flag
bool Down = false;   // command flag

//SET YOUR WIFI CREDS 
const char* myWifiSsid      = "Benny and The Jets 2.4G"; 
const char* myWifiPassword  = "eltonjohn";

//SET TO MATCH YOUR HARDWARE 
#define SERIAL_BAUD_RATE    115200

//PIN 0 is D3 ON THE CHIP 
#define IR_PIN              0
#define LED_PIN             2

//turn on/off the tv by sending IR command
void togglePower();
void muteVolume();
void volumeUp();
void volumeDown();
void blinkLed(int, int);


// ************************************************************************************
//Runs once, when device is powered on or code has just been flashed 
//
void setup()
{
  //if set wrong, your serial debugger will not be readable 
  Serial.begin(SERIAL_BAUD_RATE);

  //initialize the IR 
  IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
  IRrecv irrecv(kRecvPin);
  irsend.begin();
  irrecv.enableIRIn();  // Start the receiver

  //initialize wifi connection 
  wifi = new WifiConnection(myWifiSsid, myWifiPassword); 
  wifi->begin(); 

  //initialize wemo emulator 
  wemulator = new Wemulator(); 

  //connect to wifi 
  if (wifi->connect())
  {
    DEBUG_PRINTLN("Wifi:Connected, beginning wemulator");
    wemulator->begin();
    
    wemulator->addDevice("tv", 80, new WemoCallbackHandler(&Power)); 
    wemulator->addDevice("tv mute", 80, new WemoCallbackHandler(&Mute)); 
    wemulator->addDevice("tv volume up", 80, new WemoCallbackHandler(&Up)); 
    wemulator->addDevice("tv volume down", 80, new WemoCallbackHandler(&Down)); 

  }
}


// ************************************************************************************
// Runs constantly 
//
void loop() 
{    
  //let the wemulator listen for voice commands 
  if (wifi->isConnected)
  {
    //blinkLed(1, 100);
    wemulator->listen();
  }

  //if we've received a command, do the action 
  if (Power)
  {
    debugPrintln("TOGGLE TV COMMAND OUTGOING:");
    Power = false; 
    toggleTv(); 
    delay(100); 
  }
  if (Mute)
  {
    debugPrintln("TOGGLE MUTE COMMAND OUTGOING:");
    Mute = false; 
    muteVolume(); 
    delay(100); 
  }
  if (Up)
  {
    debugPrintln("VOLUME UP COMMAND OUTGOING:");
    Up = false; 
    volumeUp(); 
    delay(100); 
  }
  if (Down)
  {
    debugPrintln("VOLUME DOWN COMMAND OUTGOING:");
    Down = false; 
    volumeDown(); 
    delay(100); 
  }
}


// ************************************************************************************
// turn on/off the tv by sending IR command. This one is set for LG tv (NEC protocol). 
//
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

// ************************************************************************************
// blink the LED a given number of times 
//
// args: 
//  count: number of times to blink
//  delayMs: delay between blinks
void blinkLed(int count, int delayMs)
{
  for(int n=0; n<count; n++)
  {
    debugPrintln("blink");
    digitalWrite(LED_PIN, LOW);
    delay(delayMs);
    digitalWrite(LED_PIN, HIGH);
    delay(delayMs);
  }
}

