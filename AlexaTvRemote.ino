
#include "debug.h"              // Serial debugger printing
#include "WifiConnection.h"     // Wifi connection 
#include "Wemulator.h"
#include "WemoCallbackHandler.h"
#include <IRsend.h>    // IR library 


WifiConnection* wifi;           // wifi connection
IRsend* irSend;                 // infrared sender
Wemulator* wemulator;           // wemo emulator

//This is used as a crude workaround for a threading issue
bool togglePower = false;   // command flag
bool toggleMute = false;   // command flag
bool volumeUp = false;   // command flag
bool volumeDown = false;   // command flag

//SET YOUR WIFI CREDS 
const char* myWifiSsid      = "mina"; 
const char* myWifiPassword  = "HappyTime";

//SET TO MATCH YOUR HARDWARE 
#define SERIAL_BAUD_RATE    9600

//PIN 0 is D3 ON THE CHIP 
#define IR_PIN              0
#define LED_PIN             2

//turn on/off the tv by sending IR command
void toggleTv();
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
  irSend = new IRsend(IR_PIN, true);
  irSend->begin();

  //make sure that the IR pin is OFF
  // *** NOTE: HIGH and LOW may be reversed on your device. In which case, replace this call with:
  //          digitalWrite(IR_PIN, LOW);
  digitalWrite(IR_PIN, HIGH);

  //initialize wifi connection 
  wifi = new WifiConnection(myWifiSsid, myWifiPassword); 
  wifi->begin(); 

  //initialize wemo emulator 
  wemulator = new Wemulator(); 

  //connect to wifi 
  if (wifi->connect())
  {
    wemulator->begin();
    
    wemulator->addDevice("tv", 80, new WemoCallbackHandler(&togglePower)); 
    wemulator->addDevice("tv mute", 80, new WemoCallbackHandler(&toggleMute)); 
    wemulator->addDevice("tv volume up", 80, new WemoCallbackHandler(&volumeUp)); 
    wemulator->addDevice("tv volume down", 80, new WemoCallbackHandler(&volumeDown)); 

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
  if (togglePower)
  {
    debugPrintln("TOGGLE TV COMMAND OUTGOING:");
    togglePower = false; 
    toggleTv(); 
    delay(100); 
  }
  if (toggleMute)
  {
    debugPrintln("TOGGLE MUTE COMMAND OUTGOING:");
    toggleMute = false; 
    toggleMute(); 
    delay(100); 
  }
  if (volumeUp)
  {
    debugPrintln("VOLUME UP COMMAND OUTGOING:");
    volumeUp = false; 
    volumeUp(); 
    delay(100); 
  }
  if (volumeDown)
  {
    debugPrintln("VOLUME DOWN COMMAND OUTGOING:");
    volumeDown = false; 
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
  //irSend->sendLG(0x00FFE01FUL, 32, 2); 

  // 0x20DF10EF is the command code 
  // 32 is the number of bits in the command 
  // 2 is the number of times it will transmit/repeat the command. Sometimes 1 time is not 
  //    enough to get the command across. you can change this number if you wish 
  //irSend->sendNEC(0x20DF10EF, 32, 2); 
}

void muteVolume()
{
  debugPrintln("Sending IR command to toggle mute"); 
  //irSend->sendLG(0x00FFE01FUL, 32, 2); 

  // 0x20DF10EF is the command code 
  // 32 is the number of bits in the command 
  // 2 is the number of times it will transmit/repeat the command. Sometimes 1 time is not 
  //    enough to get the command across. you can change this number if you wish 
  //irSend->sendNEC(0x20DF10EF, 32, 2); 
}

void volumeUp()
{
  debugPrintln("Sending IR command to turn volume up"); 
  //irSend->sendLG(0x00FFE01FUL, 32, 2); 

  // 0x20DF10EF is the command code 
  // 32 is the number of bits in the command 
  // 2 is the number of times it will transmit/repeat the command. Sometimes 1 time is not 
  //    enough to get the command across. you can change this number if you wish 
  //irSend->sendNEC(0x20DF10EF, 32, 2); 
}

void volumeDown()
{
  debugPrintln("Sending IR command to turn volume down"); 
  //irSend->sendLG(0x00FFE01FUL, 32, 2); 

  // 0x20DF10EF is the command code 
  // 32 is the number of bits in the command 
  // 2 is the number of times it will transmit/repeat the command. Sometimes 1 time is not 
  //    enough to get the command across. you can change this number if you wish 
  //irSend->sendNEC(0x20DF10EF, 32, 2); 
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

