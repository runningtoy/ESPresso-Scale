/////////////////////////////////////////////////////////////////

#include "Button2.h"




/////////////////////////////////////////////////////////////////

void fct_showText(String text);
void fct_powerResetTimer();
void fct_powerResetTimer(int t);


/////////////////////////////////////////////////////////////////

#define MY_LONGCLICK_MS  500
#define MY_DOUBLECLICK_MS 700



Button2 buttonTare;
Button2 buttonTimer;


/////////////////////////////////////////////////////////////////

// void click(Button2& btn) {
//     fct_powerResetTimer();
//     if (btn == buttonTare) {
//       Serial.println("A clicked");
//       //fct_showText("Tare ...");
//       // scale.tare(2, false, true, true);
//     } else if (btn == buttonTimer) {
//       Serial.println("B clicked");
//       start_timer=millis();
//     }
// }

// void doubleClick(Button2& btn) {
//     fct_powerResetTimer();
//     if (btn == buttonTare) {
//       Serial.println("A doubleClick");
//     } else if (btn == buttonTimer) {
//       timermode=!timermode;
//       Serial.println("B doubleClick");
//     }
// }


void click_buttonTare(Button2& btn) {
    fct_powerResetTimer();
    Serial.println("buttonTare click\n");
}
void longClick_buttonTare(Button2& btn) {
    fct_powerResetTimer();
    Serial.println("buttonTare long click\n");
}
void doubleClick_buttonTare(Button2& btn) {
    fct_powerResetTimer();
    Serial.println("buttonTare double click\n");
}

void click_buttonTimer(Button2& btn) {
    fct_powerResetTimer();
    Serial.println("buttonTimer click\n");
}
void longClick_buttonTimer(Button2& btn) {
    fct_powerResetTimer();
    Serial.println("buttonTimer long click\n");
}
void doubleClick_buttonTimer(Button2& btn) {
    fct_powerResetTimer();
    Serial.println("buttonTimer double click\n");
}

/////////////////////////////////////////////////////////////////

void button_setup() {
  buttonTare.begin(TARE_BUTTON_PIN,INPUT_PULLUP,false);
  buttonTimer.begin(POWER_BUTTON_PIN,INPUT_PULLUP,false);

  buttonTare.setLongClickTime(MY_LONGCLICK_MS);
  buttonTare.setDoubleClickTime(MY_DOUBLECLICK_MS);
  buttonTimer.setLongClickTime(MY_LONGCLICK_MS);
  buttonTimer.setDoubleClickTime(MY_DOUBLECLICK_MS);



}

/////////////////////////////////////////////////////////////////

void button_loop() {
  buttonTare.loop();
  buttonTimer.loop();
}

