#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include "Adafruit_NeoTrellis.h"

//Define the Machine ID assigned to QLab
#define MSC_ID 0

Adafruit_USBD_MIDI usb_midi;
// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

Adafruit_NeoTrellis trellis;

// Pad a string of length 'len' with nulls
void pad_with_nulls(char* s, int len) {
  int l = strlen(s);
  for( int i=l;i<len; i++) {
    s[i] = '\0';
  }
}


//put the notes in order (top left to bottom right)
int noteTable[] = {
  48,49,50,51,
  44,45,46,47,
  40,41,42,43,
  36,37,38,39
  };

//define key colors(top left to bottom right)
uint32_t colorTable_1[16] ={
  0xff0000, 0x000000, 0x000000, 0x0000ff,
  0xff00ff, 0x77ff77, 0xff7777, 0x0000ff,
  0x1a007a, 0x7777ff, 0xaa7942, 0x000000,
  0xfff000, 0xff9300, 0x000000, 0x00ff00
};



//define a callback for key presses
TrellisCallback blink(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    trellis.pixels.setPixelColor(evt.bit.NUM, 0xffffff); //on rising
    //MIDI.sendControlChange(noteTable[evt.bit.NUM], 127, 1);
    //assign MIDI Show Control (MSC) commands to the trellis keys. The relevant byte is the sixth.
    byte msg[7] = {0xF0,0x7F,MSC_ID,0x02,0x7F,0x01,0xF7};
    switch(evt.bit.NUM){
      case 3:
        msg[5] = 0x01; //MSC 'GO'
        MIDI.sendSysEx(7, msg, true);
        break;
      case 0: 
        msg[5] = 0x02; //MSC 'STOP' (Pause for QLab)
        MIDI.sendSysEx(7, msg, true);
        break;     
      case 1: 
        msg[5] = 0x03; //MSC 'RESUME'
        MIDI.sendSysEx(7, msg, true);
        break;      
      case 12: 
        msg[5] = 0x08; //MSC 'ALL_OFF' (Panic all for QLab)
        MIDI.sendSysEx(7, msg, true);
        break;   
      case 15: 
        msg[5] = 0x11; //MSC 'STANDBY_+' (Playhead to next cue)
        MIDI.sendSysEx(7, msg, true);
        break;  
      case 11: 
        msg[5] = 0x12; //MSC 'STANDBY_-' (Playhead to previous cue)
        MIDI.sendSysEx(7, msg, true);
        break;   
      case 8: 
        msg[5] = 0x0A; //MSC 'RESET'
        MIDI.sendSysEx(7, msg, true);
        break;  
      case 4: 
        msg[5] = 0x09; //MSC 'RESTORE' (Playhead to previous cue)
        MIDI.sendSysEx(7, msg, true);
        break;
      case 5: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 127, 1);
        break;
      case 6: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 127, 1);
        break;
      case 9: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 127, 1);
        break;
      case 10: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 127, 1);
        break;                                                               
    }
  }
    
   
  else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
  // or is the pad released?
    trellis.pixels.setPixelColor(evt.bit.NUM, colorTable_1[evt.bit.NUM]); //off falling
    switch (evt.bit.NUM){
      case 5: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 0, 1);
        break;
      case 6: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 0, 1);
        break;
      case 9: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 0, 1);
        break;
      case 10: 
        MIDI.sendControlChange(noteTable[evt.bit.NUM], 0, 1);
        break;
    }
    //MIDI.sendControlChange(noteTable[evt.bit.NUM], 0, 1);
  }

  // Turn on/off the neopixels!
  trellis.pixels.show();

  return 0;
}


void setup() {
  //Name the device
  char mfgstr[32] = "LSKA";  //manufacturer string
  char prodstr[32] = "NeoTrellis 4x4 - QLab"; //MIDI device name

  pad_with_nulls(mfgstr, 32);
  pad_with_nulls(prodstr, 32);
  
  USBDevice.setManufacturerDescriptor(mfgstr);
  USBDevice.setProductDescriptor(prodstr);

  
  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Attach the handleNoteOn function to the MIDI Library. It will
  // be called whenever the Bluefruit receives MIDI Note On messages.
  MIDI.setHandleControlChange(handleControlChange);


  MIDI.turnThruOff();

  Serial.begin(115200);
  
  
  if (!trellis.begin()) {
    Serial.println("Could not start trellis, check wiring?");
    while(1);
  } else {
    Serial.println("NeoPixel Trellis started");
  }

  trellis.pixels.setBrightness(10);

  //activate all keys and set callbacks
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, blink);
  }

  //do a little animation to show we're on
  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
    trellis.pixels.show();
    delay(50);
  }
  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
    trellis.pixels.show();
    delay(50);
  }

  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++){
    trellis.pixels.setPixelColor(i,colorTable_1[i]);
  }
  trellis.pixels.show();
  Serial.print("MSC Keypad ready, sending MSC commands to ID: ");
  Serial.println(MSC_ID);
}

void loop() {
  trellis.read();  // interrupt management does all the work! :)
  MIDI.read(1);
  //delay(20); //the trellis has a resolution of around 60hz
}


/******************************************/

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

void handleControlChange(byte channel, byte cc, byte value)
{
    int v = valueIndex(cc);
    if (v != -1){
      if (value > 0){
        trellis.pixels.setPixelColor(v,0xffffff);
        
      }
      else {
        trellis.pixels.setPixelColor(v,colorTable_1[v]);
      }
      trellis.pixels.show();
    }
}
   

int valueIndex(byte input){
  int i = -1;
  for (int a = 0; a < 16; a++){
    if (input == noteTable[a]){
      i = a;
    }
  }
  return i;
}
