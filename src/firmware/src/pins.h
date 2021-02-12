#ifndef __PINS_H__
#define __PINS_H__

const int BUTTON1_IN = 14;
const int BUTTON2_IN = 27;
const int BUTTON3_IN = 26;

const int BUTTON1_LED = 0;
const int BUTTON2_LED = 4;
const int BUTTON3_LED = 16;

const int MIC_PIN_BCLK = 17; // Yellow (Serial Clock)
const int MIC_PIN_LRCL = 21; // Green (Word / Channel Select)
const int MIC_PIN_SD = 22;   // Blue (Serial Data)

const int AMP_PIN_BCLK = 25; // Orange (Serial Clock)
const int AMP_PIN_LRC = 33;  // Green (Word / LeftRight / Channel Select)
const int AMP_PIN_DIN = 32;  // Blue (Serial Data)


#endif // __PINS_H__