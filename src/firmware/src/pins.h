#ifndef __PINS_H__
#define __PINS_H__

const int BUTTON1_IN = 3;       // A3 on MCP23017 (via I2C)
const int BUTTON2_IN = 4;       // A4 on MCP23017 (via I2C)
const int BUTTON3_IN = 5;       // A5 on MCP23017 (via I2C)
const int BUTTON_OFF_IN = 7;    // A7 on MCP23017 (via I2C)

const int BUTTON1_LED = 0; // A0 on MCP23017 (via I2C)
const int BUTTON2_LED = 1; // A1 on MCP23017 (via I2C)
const int BUTTON3_LED = 2; // A2 on MCP23017 (via I2C)

const int I2C_SDA = 21;
const int I2C_CLK = 22;
const int MCP23017_IRT = 35; // MCP23017 interrupt

const int MIC_PIN_BCLK = 17; // Yellow (Serial Clock)
const int MIC_PIN_SD   = 16; // Blue (Serial Data)
const int MIC_PIN_LRCL = 04; // Green (Word / Channel Select)

const int AMP_PIN_BCLK = 25; // Orange (Serial Clock)
const int AMP_PIN_LRC  = 33;  // Green (Word / LeftRight / Channel Select)
const int AMP_PIN_DIN  = 32;  // Blue (Serial Data)

const int POT_IN = 39;   // Grey 
const int LDR_PIN = 36;  // Green

#endif // __PINS_H__