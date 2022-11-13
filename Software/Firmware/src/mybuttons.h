/////////////////////////////////////////////////////////////////

#include "Button2.h"



/////////////////////////////////////////////////////////////////

void fct_showText(String text);
void fct_powerResetTimer();
void fct_powerResetTimer(int t);
void fct_timerMode();
void fct_callCalibrateScale();


/////////////////////////////////////////////////////////////////

#define MY_LONGCLICK_MS  500
#define MY_DOUBLECLICK_MS 700



Button2 buttonTare;
Button2 buttonTimer;




void click_buttonTare(Button2& btn) {
    // fct_powerResetTimer();
    scale.tare(2, false, true, false);
    // ESP_LOGV("button","buttonTare click\n");
}
void longClick_buttonTare(Button2& btn) {
    // fct_powerResetTimer();
    // scale.tare(2, false, true, false);
    // ESP_LOGV("button","buttonTare long click\n");
}
void doubleClick_buttonTare(Button2& btn) {
    // fct_powerResetTimer();
    // ESP_LOGV("button","buttonTare double click\n");
}

void click_buttonTimer(Button2& btn) {
    // fct_powerResetTimer();
    fct_timerMode();
    // ESP_LOGV("button","buttonTimer click\n");
}
void longClick_buttonTimer(Button2& btn) {
    // fct_powerResetTimer();
    fct_timerMode();
    // ESP_LOGV("button","buttonTimer long click\n");
}
void doubleClick_buttonTimer(Button2& btn) {
    // fct_powerResetTimer();
    // ESP_LOGV("button","buttonTimer double click\n");
}

int counter = 0;
unsigned long now = 0;

void pressed(Button2 &btn)
{   
    //reset auto power off timer
    fct_powerResetTimer();

    //which buttons pressed
    if (btn == buttonTare)
    {
        counter = (counter | 1);
    }
    if (btn == buttonTimer)
    {
        counter = (counter | 2);
    }
    if (counter == 3)
    {
        now = millis();
    }
    ESP_LOGV("button","%d",counter);
}

void released(Button2 &btn)
{
    if (counter == 3)   //if both buttons pressed at the same time
    {
        if ((millis() - now) > 6000) //if both buttons pressed at the same time for longer then xx ms clear and reset NVS
        {
            ESP_LOGV("monitoring","clearNVS");
            display.clearDisplay();
            NVS.eraseAll();
            NVS.setInt("calFactorULong",0);
            NVS.setInt("validitycheck",32767); 
            ESPHardRestart();
        }
        if ((millis() - now) > 2000) //if both buttons pressed at the same time for longer then xx ms then enter calibration mode
        {
            ESP_LOGV("button","%d",(millis() - now));
            fct_callCalibrateScale();
        }
    }
    counter = 0;
}

/////////////////////////////////////////////////////////////////

void button_setup() {
  buttonTimer.begin(POWER_BUTTON_PIN,INPUT_PULLUP,false);
  
  buttonTimer.setLongClickTime(MY_LONGCLICK_MS);
  buttonTimer.setDoubleClickTime(MY_DOUBLECLICK_MS);
  
  buttonTimer.setLongClickHandler(longClick_buttonTimer);  
  buttonTimer.setClickHandler(click_buttonTimer);  
  //-----------------------------------------------
  buttonTare.begin(TARE_BUTTON_PIN,INPUT_PULLUP,false);
  
  buttonTare.setLongClickTime(MY_LONGCLICK_MS);
  buttonTare.setDoubleClickTime(MY_DOUBLECLICK_MS);
  
  buttonTare.setLongClickHandler(longClick_buttonTare);  
  buttonTare.setClickHandler(click_buttonTare);  


  buttonTimer.setPressedHandler(pressed);
  buttonTare.setPressedHandler(pressed);

  buttonTimer.setReleasedHandler(released);
  buttonTare.setReleasedHandler(released);  

}

/////////////////////////////////////////////////////////////////

void button_loop() {
  buttonTare.loop();
  buttonTimer.loop();
}


bool wifiActivationRequested(){
    button_loop();
    return buttonTare.isPressed();
}