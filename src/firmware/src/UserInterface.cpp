#include "UserInterface.h"

#include "pins.h"
#include <OneButton.h>

OneButton button1 = OneButton(BUTTON1_IN, false, false);
OneButton button2 = OneButton(BUTTON2_IN, false, false);
OneButton button3 = OneButton(BUTTON3_IN, false, false);

void UserInterface::setup() 
{
  Serial.begin(115200);

  button1.attachClick([]() {
      Serial.println("Clicked Button 1");
  });
  button2.attachClick([]() {
      Serial.println("Clicked Button 2");
  });
  button3.attachClick([]() {
      Serial.println("Clicked Button 3");
  });

//   button1.attachLongPressStart(longPressStart);
//   button1.attachLongPressStop(longPressEnd);    
}

void UserInterface::loop() 
{
  button1.tick();
  button2.tick();
  button3.tick();
}

// void clicked() {
  
// }

// void longPressStart() {
//   Serial.println("Long Press Started");
// }

// void longPressEnd() {
//   Serial.print("Long Press Ended after ");
//   Serial.print(button1.getPressedTicks());
//   Serial.println(" ms");
// }