#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>


#include <Adafruit_GFX.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Adafruit_SSD1306.h>
#include "defines.h"
#include "scale.h"
#define ADS1232
#include "MONITORING.h"
SCALE scale = SCALE(ADC_PDWN_PIN, ADC_SCLK_PIN, ADC_DOUT_PIN, ADC_A0_PIN, ADC_SPEED_PIN, ADC_GAIN1_PIN, ADC_GAIN0_PIN, ADC_TEMP_PIN);
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);
Ticker powerTicker;
uint32_t powertimer=POWERDOWNTIMER;
MONITORING monitoring = MONITORING(23,36); //enable mosfet pin = 23, vin on gpio36
bool timermode=false;
uint32_t start_timer=0;

#include "mybuttons.h"





float fct_roundToDecimal(double value, int dec)
{
  double mlt = powf( 10.0f, dec );
  value = roundf( value * mlt ) / mlt;
  return (float)value;
}

void fct_powerDown(){
  digitalWrite(POWERLATCH, LOW);
  digitalWrite(LED, LOW);
}

void fct_powerDownTicker(){
  powertimer--;
  if(powertimer<1){
    fct_powerDown();
  }
}

void fct_powerResetTimer(int t){
   powertimer=t;
}

void fct_powerResetTimer(){
   fct_powerResetTimer(POWERDOWNTIMER);
}

void fct_powerUp(){
  pinMode(POWERLATCH, OUTPUT);
  digitalWrite(POWERLATCH, HIGH);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // ledcAttachPin(LED, 0);
  // ledcSetup(0, 4000, 8); 
  // ledcWrite(0, 10);
}



void fct_initScale()
{
  pinMode(ADC_LDO_EN_PIN, OUTPUT);
  digitalWrite(ADC_LDO_EN_PIN, ADC_LDO_ENABLE);
  //settings ----- todo
  const uint8_t DEFAULT_ADC_SPEED = 10; //10 or 80
  //--------
  scale.begin(0, 128, DEFAULT_ADC_SPEED);
  scale.calibrateADC();


  // //Apply scale options we got from EEPROM
  // scale.zeroRange = (float)settings.zeroRange / 100.0;
  // scale.zeroTracking = (float)settings.zeroTracking / 100.0;
  // scale.fakeStabilityRange = (float)settings.fakeRange / 100.0;
  // scale.decimalDigits = settings.decimalDigits;
  // scale.setSensitivity(settings.sensitivity);
  // scale.setSmoothing(settings.smoothing);
  // scale.setCalFactor(settings.calFactorULong / 10.0);
  // scale.autoTare = settings.autoTare;
  // scale.autoTareNegative = settings.autoTareNegative;
  // scale.autoStartTimer = settings.autoStartTimer;
  // scale.autoStopTimer = settings.autoStopTimer;
  // scale.timerDelay = settings.timerDelay * 1000;
  // scale.timerStartWeight = (float)settings.timerStartWeight / 10.0;
  // scale.timerStopWeight = (float)settings.timerStopWeight;
  // scale.rocStartTimer = (float)settings.rocStartTimer / 10.0;
  // scale.rocStopTimer = (float)settings.rocStopTimer / 10.0;
  // scale.tare(2, false, false, true);
}

void fct_showText(String text) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  // display.setFont(&FreeSans12pt7b);
  display.setTextSize(2);
  display.setCursor(10, 9);
  display.println(text);
  display.display();      // Show initial text
  delay(100);
}

void fct_showText(String text,String text2) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  // display.setFont(&FreeSans12pt7b);
  display.setTextSize(2);
  display.setCursor(10, 2);
  display.println(text);
  display.setTextSize(1);
  display.setCursor(10, DISPLAY_HEIGHT-8);
  display.println(text2);
  display.display();      // Show initial text
  delay(100);
}

void fct_showNumber(double f) {
   fct_showText(String(fct_roundToDecimal(f,2)));
}

void fct_showGrammes(double f) {
   fct_showText(String(fct_roundToDecimal(f,2))+" g","Bat: "+String(fct_roundToDecimal(vinVoltage,1)));
}

void fct_showTime(double f) {
   fct_showText(String(fct_roundToDecimal(f,2))+" s","Bat: "+String(fct_roundToDecimal(vinVoltage,1)));
}

void fct_setupDisplay(){
  Wire.begin(DISPLAY_MOSI_PIN, DISPLAY_CLK_PIN);
  Serial.println("Wire Init");
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)) {
        Serial.println("SSD1306 allocation failed");
      }
  display.display();
  display.clearDisplay();
}


void setup() {
  fct_powerUp();
  Serial.begin(115200);
  fct_setupDisplay();
  Serial.println("Setup: fct_setupDisplay");
  //---------------------
  fct_showText("Init...");
  button_setup();
  fct_powerResetTimer();
  powerTicker.attach(1, fct_powerDownTicker);
  // ---------------------
  fct_showText("Scale...");
  fct_initScale();
  Serial.println("Setup: fct_initScale");
  //---------------------
  fct_showText("Tare...");
  scale.tare(2, false, true, true);
  Serial.println("Setup: scale.tare");
  //---------------------

  
}

uint32_t u_time=0;
uint32_t lastVinRead=0;

void loop()
{
  button_loop();
  if ((millis() - u_time >= 30))
  {
      double v=scale.readUnits(1);
      fct_showGrammes(v);
      // Serial.print(v);Serial.println(": readUnits");
      u_time = millis(); 
  }

  // if ((millis() - u_time >= 50) && (timermode))
  // {
  //     u_time = millis(); 
  //     fct_showTime(u_time-start_timer);
  // }

  
  if (millis() - lastVinRead >=(batReadInterval*1000)) {
        vinVoltage = monitoring.getVoltage();
        // resolutionLevel = monitoring.getResolutionLevel(vinVoltage);
        lastVinRead = millis(); 
        // Serial.print(vinVoltage);Serial.print("V (");Serial.print(resolutionLevel);Serial.println(")");
  }

  // put your main code here, to run repeatedly:
}