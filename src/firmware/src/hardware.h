#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// ++++++++++++++++++++++++++++++++++
// +    HARDWARE CAPABILITIES       +
// ++++++++++++++++++++++++++++++++++

//#define HAS_BUTTON_PANEL


// ++++++++++++++++++++++++++++++++++
// +    HARDWARE CONFIGURATION      +
// ++++++++++++++++++++++++++++++++++

// ButtonPanel
// -----------

const int BUTTON1_IN = 3;     // A3 on MCP23017 (via I2C)
const int BUTTON2_IN = 4;     // A4 on MCP23017 (via I2C)
const int BUTTON3_IN = 5;     // A5 on MCP23017 (via I2C)
const int BUTTON_OFF_IN = 7;  // A7 on MCP23017 (via I2C)

const int BUTTON1_LED = 6;    // A0 on MCP23017 (via I2C)
const int BUTTON2_LED = 1;    // A1 on MCP23017 (via I2C)
const int BUTTON3_LED = 2;    // A2 on MCP23017 (via I2C)

const int I2C_SDA = 21;       // MCP23017 data channel (I2C)
const int I2C_CLK = 22;       // MCP23017 clock (I2C)
const int MCP23017_IRT = 35;  // MCP23017 interrupt input to ESP32


// Microphone
// ----------

const int MIC_PIN_BCLK = 17;  // Yellow (Serial Clock)
const int MIC_PIN_SD   = 16;  // Blue (Serial Data)
const int MIC_PIN_LRCL = 04;  // Green (Word / Channel Select)


// Speaker
// -------

const int AMP_PIN_BCLK = 25;  // Orange (Serial Clock)
const int AMP_PIN_LRC  = 33;  // Green (Word / LeftRight / Channel Select)
const int AMP_PIN_DIN  = 32;  // Blue (Serial Data)

// Volume  (Potentiometer)
// -----------------------
const int POT_IN = 39;   // Grey 

// Anbience Monitor
// ----------------
const int LDR_PIN = 36;  // Green


// +++++++++++++++++++++++++++++++++++++++++++++++++
// +    DEFINE TYPES BASED ON HW CAPABILITIES      +
// +++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef HAS_BUTTON_PANEL
#define BUTTON_PANEL_TYPE ButtonPanel
#else
#include "ui/NoOpsButtonPanel.h"
#define BUTTON_PANEL_TYPE NoOpsButtonPanel
#endif // HAS_BUTTON_PANEL

#endif // __HARDWARE_H__