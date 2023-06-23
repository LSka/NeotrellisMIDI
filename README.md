# NeotrellisMIDI
Arduino sketches for using the Adafruit 4x4 NeoTrellis Feather M4 kit as a MIDI controller

Once you upload any of the two sketches, the device will be recognized as a MIDI controller. 

*NeoTrellis4x4_MIDI* is a simple chromatic keyboard, ouputting notes from C3 (36) to D#4 (51) at velocity 127

*Neotrellis4x4_MIDI_QLAB* is a custom controller for QLab, outputting MSC (MIDI Show Control) commands.

# Requirements
Adafruit seesaw library https://github.com/adafruit/Adafruit_Seesaw

MIDI Library by François Best, lathoub https://github.com/FortySevenEffects/arduino_midi_library

# Setup
Follow this guide to install the board definition in Arduino IDE: https://learn.adafruit.com/adafruit-feather-m4-express-atsamd51/setup
then install the two libraries above in the Arduino IDE Library Manager
