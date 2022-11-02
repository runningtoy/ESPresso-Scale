/////////////////////////////////////////////////////////////////

// #include "Button2.h";
#include <ButtonHandler.h>



/////////////////////////////////////////////////////////////////

void fct_showText(String text);
void fct_powerResetTimer();
void fct_powerResetTimer(int t);


/////////////////////////////////////////////////////////////////

#define SHORT_CLICK_EVENT_INTERVAL 150
#define DOUBLE_CLICK_EVENT_INTERVAL 300
#define LONG_CLICK_EVENT_INTERVAL 500
#define RATTLE_VALUE 200

ButtonHandler buttonTare(
  TARE_BUTTON_PIN,
  SHORT_CLICK_EVENT_INTERVAL,
  DOUBLE_CLICK_EVENT_INTERVAL,
  LONG_CLICK_EVENT_INTERVAL,
  RATTLE_VALUE
);

ButtonHandler buttonTimer(
  POWER_BUTTON_PIN,
  SHORT_CLICK_EVENT_INTERVAL,
  DOUBLE_CLICK_EVENT_INTERVAL,
  LONG_CLICK_EVENT_INTERVAL,
  RATTLE_VALUE
);



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

/////////////////////////////////////////////////////////////////

void button_setup() {}

/////////////////////////////////////////////////////////////////

void button_loop() {
  buttonTare.processEvents();
  buttonTimer.processEvents();
  switch (buttonTare.getEvent()) {
    case ButtonHandler::EVENT_SHORT_CLICK:
      fct_powerResetTimer();
      Serial.println("buttonTare short");
      scale.tare(2, false, true, true); 
      break;
    case ButtonHandler::EVENT_DOUBLE_CLICK:
      fct_powerResetTimer();
      Serial.println("buttonTare double");
      break;
    case ButtonHandler::EVENT_LONG_CLICK:
      fct_powerResetTimer();
      Serial.println("buttonTare long");
      // scale.tare(0, false, false, false);
      break;
  }
  switch (buttonTimer.getEvent()) {
    case ButtonHandler::EVENT_SHORT_CLICK:
      fct_powerResetTimer();
      start_timer=millis();
      Serial.println("buttonTimer short");
      break;
    case ButtonHandler::EVENT_DOUBLE_CLICK:
      fct_powerResetTimer();
      Serial.println("buttonTimer double");
      timermode=!timermode;
      break;
    case ButtonHandler::EVENT_LONG_CLICK:
      fct_powerResetTimer();
      Serial.println("buttonTimer long");
      break;
  }
}

