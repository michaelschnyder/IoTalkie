#ifndef __PINS_H__
#define __PINS_H__

const int BUTTON1_IN = 99;
const int BUTTON2_IN = 99;
const int BUTTON3_IN = 99;

const int BUTTON1_LED = 99;
const int BUTTON2_LED = 99;
const int BUTTON3_LED = 99;

const int MIC_PIN_BCLK = 17; // Yellow (Serial Clock)
const int MIC_PIN_SD   = 16; // Blue (Serial Data)
const int MIC_PIN_LRCL = 04; // Green (Word / Channel Select)

const int AMP_PIN_BCLK = 25; // Orange (Serial Clock)
const int AMP_PIN_LRC  = 33;  // Green (Word / LeftRight / Channel Select)
const int AMP_PIN_DIN  = 32;  // Blue (Serial Data)

const int BUTTON_OFF_IN = 99;
const int POT_IN = 39;   // Grey 
const int LDR_PIN = 36;  // Green

#endif // __PINS_H__