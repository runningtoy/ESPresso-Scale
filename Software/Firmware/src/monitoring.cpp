/*  
  https://gitlab.com/jousis/espresso-scale
*/

#include "monitoring.h"


MONITORING::MONITORING(uint8_t enable_pin, uint8_t read_pin)
{
  enablePin = enable_pin;
  readPin = read_pin;  
  pinMode(enablePin, OUTPUT);
  pinMode(readPin, INPUT_PULLUP);
} 

float MONITORING::roundToDecimal(double value, int dec)
{
  double mlt = powf( 10.0f, dec );
  value = roundf( value * mlt ) / mlt;
  return (float)value;
}

double MONITORING::readRaw() {
  //enable mosfet
  digitalWrite(enablePin, HIGH);
  double reading = analogRead(readPin);
  digitalWrite(enablePin, LOW);
  return reading;
}

double MONITORING::readVoltage(){
  double reading = readRaw();
  if(reading < 1 || reading > 4095) return 0;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}

float MONITORING::getVoltage() {
  double mlt = MULTIPLIER;
  if (mlt <= 0 ) {
    if (R18 > 0) {
      mlt = (float)(R17+R18)/R18;
    } else {
      //R18 == 0 ??? no way
      mlt = 2; //default with 10K/10K
    }
  }
  return roundToDecimal(readVoltage()*mlt,2);
}

String MONITORING::getResolutionLevel(float voltage) {
  if (voltage == -1) {
    voltage = getVoltage();
  }
  if (voltage < VIN_MIN) {
    return "MIN";
  } else if (voltage < VIN_LOW) {
    return "LOW";
  } else if (voltage < VIN_OK) {
    return "OK";
  } else if (voltage < VIN_GOOD) {
    return "GOOD";
  } else {
    return "MAX";
  }
}

double MONITORING::getRawRead() {
  return readRaw();
}
