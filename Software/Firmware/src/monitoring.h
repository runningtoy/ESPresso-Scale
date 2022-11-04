/*  
  ESPresso Scale
  Created by John Sartzetakis, Jan 2019
  Released into the public domain.
  https://gitlab.com/jousis/espresso-scale
*/

#ifndef MONITORING_h
#define MONITORING_h

#include <Arduino.h>


const float VIN_MIN = 3.35; //anything < 3.35 == vin_min
const float VIN_LOW = 3.50;
const float VIN_OK = 3.6;
const float VIN_GOOD = 5.2;

//MULTIPLIER depends on your voltage divider Resistors. Theoretically is (R1+R2)/R2.
//if you want to fine tune it, change to anything >0.
//Otherwise it will be calculated automatically depending on the R1/R2 values you set
//const float MULTIPLIER = 1.985; 
const float MULTIPLIER = 0;
//const uint16_t R17 = 28; //>12V ldo version
const uint16_t R17 = 10; //standard USB powered version
const uint16_t R18 = 10;

class MONITORING
{  
  public:
    MONITORING(uint8_t enable_pin, uint8_t read_pin);
    String getResolutionLevel(float voltage = -1);
    float getVoltage();
    double getRawRead();
    int getSOC();
    double readVoltage();

    
  protected:    
    float roundToDecimal(double value, int dec);
    double readRaw();

    uint8_t enablePin;
    uint8_t readPin;
  
};


#endif
