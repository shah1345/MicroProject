/////////////////////////////////////////////////////////////////

#include "Button2.h"

/////////////////////////////////////////////////////////////////




#define B_High  2
#define B_Mid  0
#define B_Low  0
#define B_Start  0

  Button2 buttonH, buttonM, buttonL, buttonS;

  buttonH.begin(B_High);
  buttonH.setClickHandler(click);

  buttonM.begin(B_Mid);
  buttonM.setClickHandler(click);

  buttonL.begin(B_Low);
  buttonL.setClickHandler(click);

  buttonS.begin(B_Start);
  buttonS.setClickHandler(click);

/////////////////////////////////////////////////////////////////

Button2 buttonA, buttonB;

/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n\nMultiple Buttons Demo");
  
  buttonA.begin(BUTTON_A_PIN);
  buttonA.setClickHandler(click);

  buttonB.begin(BUTTON_B_PIN);
  buttonB.setClickHandler(click);
}

/////////////////////////////////////////////////////////////////
buttonH, buttonM, buttonL, buttonS;
void loop() {
  buttonH.loop();
  buttonM.loop();
  buttonL.loop();
  buttonS.loop();
}

/////////////////////////////////////////////////////////////////

void click(Button2& btn) {
    if (btn == buttonH) {
      Serial.println("High clicked");
    } else if (btn == buttonM) {
      Serial.println("Mediam clicked");
    } else if (btn == buttonL) {
      Serial.println("Low clicked");
    } else if (btn == buttonS) {
      Serial.println("Start clicked");
    }
}

/////////////////////////////////////////////////////////////////
