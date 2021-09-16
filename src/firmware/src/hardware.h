#ifndef __HARDWARE_H__
#define __HARDWARE_H__

// ++++++++++++++++++++++++++++++++++
// +    HARDWARE CAPABILITIES       +
// ++++++++++++++++++++++++++++++++++

//#define HAS_BUTTON_PANEL
//#define HAS_SPEAKER

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

// Screen  <-- note that any changes need to be reflected in the display driver class as well for now :)
// ------
const int SCRN_BUSY = 26;     // ePaper Busy indicator (SPI MISO aquivalent)
const int SCRN_RSET = 13;     // ePaper Reset switch
const int SCRN_DC   = 12;     // ePaper Data/Command selection
const int SCRN_CS   = 15;     // SPI Channel Chip Selection for ePaper
const int SCRN_SCK  = 14;     // SPI Channel Click
const int SCRN_SDI  = 27;     // SPI Channel MOSI Pin
const int SCRN_SPI_CHAN = 2;  // HSPI

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

#ifdef HAS_SPEAKER
#include "audio/AudioPlayer.h"
#define AUDIO_PLAYER_TYPE AudioPlayer
#else
#include "audio/NoOpsAudioPlayer.h"
#define AUDIO_PLAYER_TYPE NoOpsAudioPlayer
#endif // HAS_BUTTON_PANEL


#endif // __HARDWARE_H__