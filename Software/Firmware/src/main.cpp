#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>


#include <Adafruit_GFX.h>
#include "FreeSansBold9pt7b.h"
#include <Adafruit_SSD1306.h>
#include "defines.h"
#include "scale.h"
#define ADS1232
#include "MONITORING.h"
#include "ArduinoNvs.h"

SCALE scale = SCALE(ADC_PDWN_PIN, ADC_SCLK_PIN, ADC_DOUT_PIN, ADC_A0_PIN, ADC_SPEED_PIN, ADC_GAIN1_PIN, ADC_GAIN0_PIN, ADC_TEMP_PIN);
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);
Ticker powerTicker;
Ticker logoTicker;
uint32_t powertimer=POWERDOWNTIMER;
MONITORING monitoring = MONITORING(23,36); //enable mosfet pin = 23, vin on gpio36
bool timermode=false;
uint32_t start_timer=0;
uint32_t calFactorULong=CALFACTORDEFAULT;
bool do_calibration=false;

#include "mybuttons.h"

int count=26; 
void fct_bootLogo(){
  display.clearDisplay();
  // display count animation image
  display.drawBitmap(int((128-32)/2), 0, waitLogo[count], 32, 32, WHITE);
  display.display();
  count--;
  if (count<0) count = 26;
}

float fct_roundToDecimal(double value, int dec)
{
  double mlt = powf( 10.0f, dec );
  value = roundf( value * mlt ) / mlt;
  return (float)value;
}

void fct_powerDown(){
  NVS.close();
  digitalWrite(POWERLATCH, LOW);
  digitalWrite(LED, LOW);
}

void fct_powerResetTimer(int t){
   powertimer=t;
}

void fct_powerResetTimer(){
   fct_powerResetTimer(POWERDOWNTIMER);
}

void fct_powerDownTicker(){
  if(!scale.gethasSettled()){fct_powerResetTimer();}
  powertimer--;
  if(powertimer<1){
    fct_powerDown();
  }
}

void fct_timerMode(){
   start_timer=millis();
   timermode=!timermode;

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

bool inRange(uint32_t val, uint32_t value, double range)
{
  uint32_t minimum=(uint32_t)(value*(1-range));
  uint32_t maximum=(uint32_t)(value*(1+range));
  return ((minimum <= val) && (val <= maximum));
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


  uint32_t _calFactorULong_=NVS.getInt("calFactorULong");
  //set CalcFactor only if in a plausible
  if(inRange(_calFactorULong_,NVS.getInt("validitycheck"),0.05)){
    calFactorULong=_calFactorULong_;
    scale.setCalFactor(calFactorULong/100.0);
    Serial.println("setCalFactor: valid");
  }
  else{
    Serial.println("setCalFactor: not valid");
  }

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

charging getSOCIdx() {
  if (soc_battery < 10) {
    return BAT_EMPTY;
  } else if (soc_battery < 76) {
    return BAT_50;
  } else if (soc_battery < 101) {
    return BAT_100;
  } else {
    return BAT_EMPTY;
  }
  return BAT_EMPTY;
}


void fct_showText(String text,String text2) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSansBold9pt7b);
  display.setTextSize(1);
  display.setCursor(10, DISPLAY_HEIGHT-19);  
  display.println(text);

  if(timermode){
     display.fillRect(0,DISPLAY_HEIGHT/2,DISPLAY_WIDTH,DISPLAY_HEIGHT,SSD1306_WHITE);
     display.setTextColor(SSD1306_BLACK);
     display.setFont(&FreeSansBold9pt7b);
     display.setTextSize(1);
     display.setCursor(10, DISPLAY_HEIGHT-2);
     display.drawBitmap(int((128-25)), 16, battery[getSOCIdx()], 16, 16, SSD1306_BLACK);
  }
  else{
     display.setFont();
     display.setTextSize(1);
     display.setCursor(10, DISPLAY_HEIGHT-8);
     display.drawBitmap(int((128-25)), 16, battery[getSOCIdx()], 16, 16, SSD1306_WHITE);
  }
  display.println(text2);
  display.display();      // Show initial text
  display.setFont();
  display.setTextColor(SSD1306_WHITE);
  delay(100);
}


void fct_calibrationDisplay(){
  switch (scale.getCalibrationStatus())
  {
  case calibrationStatus::ERROR:
    fct_showText("ERROR!.","Timeout");
    break;
  case calibrationStatus::START:
    fct_showText("Calibration","no weight!");
    break;
  case calibrationStatus::PLACE:
    fct_showText("Calib. 1/4","place 100g");
    break;
  case calibrationStatus::STAGE_1:
    fct_showText("Calib. 2/4","");
    break;
  case calibrationStatus::STAGE_2:
    fct_showText("Calib. 3/4","");
    break;
  case calibrationStatus::STAGE_3:
    fct_showText("Calib. 4/4","");
    break;
  case calibrationStatus::FINISHED:
    fct_showText("Calib. Done","Finished");
    break;
  default:
    break;
  }
}

void fct_callCalibrateScale(){
  do_calibration=true;
}

void fct_calibrateScale(){
  timermode=false;
  logoTicker.attach_ms(40, fct_calibrationDisplay);
  Serial.println("Start Calibration");
  for(int i=0;i<20;i++){delay(100);}
  scale.calibrate(CALIBRATIONWEIGHT,120000,0.05);
  calFactorULong=(uint32_t)(scale.getCalFactor()*100.0);
  NVS.setInt("calFactorULong",calFactorULong);
  NVS.setInt("validitycheck",calFactorULong); 
  Serial.println("Calibration Done:: calFactorULong: "+String(calFactorULong));
    
  for(int i=0;i<20;i++){delay(100);}
  logoTicker.detach();
  do_calibration=false;
}

// t is time in seconds = millis()/1000;
char * TimeToString(unsigned long t)
{
 static char str[12];
 
 unsigned long allSeconds=t/1000;
 unsigned long runMillis=t%1000;

 sprintf(str, "%03d.%03d\'\'", allSeconds, runMillis);
 return str;
}

void fct_showGrammes(double f) {
   if(timermode){
      fct_showText(String(fct_roundToDecimal(f,2))+" g",String(TimeToString(millis()-start_timer)));
   }
   else
   {
      fct_showText(String(fct_roundToDecimal(f,2))+" g","Bat: "+String(soc_battery)+"%");
   }
}

void fct_showTime(double f) {
   fct_showText(String(fct_roundToDecimal(f,2))+" s","Bat: "+String(soc_battery)+"%");
}

void fct_setupDisplay(){
  Wire.begin(DISPLAY_MOSI_PIN, DISPLAY_CLK_PIN);
  Serial.println("Wire Init");
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)) {
        Serial.println("SSD1306 allocation failed");
      }
  display.display();
  display.setRotation(2);
  display.clearDisplay();
}


void setup() {
  fct_powerUp();
  Serial.begin(115200);
  fct_setupDisplay();
  Serial.println("Setup: fct_setupDisplay");
  //---------------------
  logoTicker.attach_ms(40, fct_bootLogo);
  button_setup();
  fct_powerResetTimer();
  powerTicker.attach(1, fct_powerDownTicker);
  // ---------------------
  NVS.begin();
  // ---------------------
  fct_initScale();
  Serial.println("Setup: fct_initScale");
  //---------------------
  
  scale.tare(2, false, true, true);
  Serial.println("Setup: scale.tare");

  //---------------------
  for(int i=0;i<3;i++){
      soc_battery = monitoring.getSOC();
      delay(30);
  }  
  //---------------------
  logoTicker.detach();
  //---------------------

  
}

uint32_t u_time=0;
uint32_t lastVinRead=0;

void loop()
{
  button_loop();

  //---- Scale  Loop
  if ((millis() - u_time >= 30))
  {
      fct_showGrammes(scale.readUnits(1));
      u_time = millis(); 
  }

  //---- Battery Loop
  if (millis() - lastVinRead >=(batReadInterval*1000)) {
        soc_battery = monitoring.getSOC();
        lastVinRead = millis(); 
  }


  if(do_calibration){
    fct_calibrateScale();
  }

  // put your main code here, to run repeatedly:
}