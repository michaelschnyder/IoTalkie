#include "UserInterface.h"
#include "pins.h"

OneButton button1 = OneButton(BUTTON1_IN, false, false);
OneButton button2 = OneButton(BUTTON2_IN, false, false);
OneButton button3 = OneButton(BUTTON3_IN, false, false);

void raiseButtonEvent(ButtonContext *ctx, Action) {
  ButtonEvent evt = {ctx->buttonId, Clicked, ctx->button.getPressedTicks()};

  UserInterface* uiCast = (UserInterface *)ctx->ui;

  if (uiCast->buttonActionCallback != NULL) {
    uiCast->buttonActionCallback(evt);
  }
}
void fClicked(void *s)
{
  ButtonContext* ctx = (ButtonContext *)s;
  Serial.print("Clicked: ");
  Serial.println(ctx->buttonId);
  raiseButtonEvent(ctx, Clicked);
}

void fLongPressStart(void *s)
{
  ButtonContext* ctx = (ButtonContext *)s;
  Serial.print("Long Press Start: ");
  Serial.println(ctx->buttonId);
  raiseButtonEvent(ctx, LongPressStart);
}

void fLongPressEnd(void *s)
{
  ButtonContext* ctx = (ButtonContext *)s;
  Serial.print("Long Press End: ");
  Serial.println(ctx->buttonId);
  raiseButtonEvent(ctx, Clicked);
}

void UserInterface::setup() 
{
  btnCtx1 = (ButtonContext){ 1, button1, this};
  btnCtx2 = (ButtonContext){ 2, button2, this};
  btnCtx3 = (ButtonContext){ 3, button3, this};

  button1.attachClick(fClicked, &btnCtx1);
  button1.attachLongPressStart(fLongPressStart, &btnCtx1);
  button1.attachLongPressStop(fLongPressEnd, &btnCtx1);    

  button2.attachClick(fClicked, &btnCtx2);
  button2.attachLongPressStart(fLongPressStart, &btnCtx2);
  button2.attachLongPressStop(fLongPressEnd, &btnCtx2);    
  
  button3.attachClick(fClicked, &btnCtx3);
  button3.attachLongPressStart(fLongPressStart, &btnCtx3);
  button3.attachLongPressStop(fLongPressEnd, &btnCtx3);    
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
void UserInterface::raiseEvent(int buttonId, Action action, long duration) 
{
    
}
