/*  
  https://gitlab.com/jousis/espresso-scale
*/

#include "SCALE.h"
//
//#define SERIAL_IF
//#define DEBUG

SCALE::SCALE(uint8_t pdwn, uint8_t sclk, uint8_t dout) : adc( new ADS1232(pdwn, sclk, dout))
{
  
} 

SCALE::SCALE(uint8_t pdwn, uint8_t sclk, uint8_t dout, uint8_t a0, uint8_t spd, uint8_t gain1, uint8_t gain0, uint8_t temp) : adc ( new ADS1232(pdwn, sclk, dout, a0, spd, gain1, gain0, temp))
{
  
}



void SCALE::begin(uint8_t channel, uint8_t gain, uint8_t speed) 
{
  adc->begin(channel,gain,speed);  
}




void SCALE::powerOn()
{
  adc->powerOn();
}

void SCALE::powerOff()
{
  adc->powerOff();
}


void SCALE::setSpeed(uint8_t sps) //10 or 80 sps , default 10
{
  adc->setSpeed(sps);
}
uint8_t SCALE::getSpeed()
{
  return(adc->getSpeed());
}


int SCALE::getAdcActualSPS()
{  
  return adc->actualSPS;
}

void SCALE::setGain(uint8_t gain)
{  
  adc->setGain(gain);
}

//void SCALE::setScaleMode(uint8_t scalemode) {
//  scaleMode=scalemode;
//  switch (scalemode) {
//    case 1 :
//      autoTare = true;
//      break;
//    default :
//      autoTare = false;    
//  }
//}

void SCALE::setSensitivity(uint8_t sensitivity)
{
  int datasetsize = 1;
  if (sensitivity <= 0) { sensitivity = 1; }
  datasetsize = (int) DATA_SET_MAX/sensitivity;
  adc->setDataSetSize(datasetsize);
}

void SCALE::setSmoothing(uint8_t smoothing) {
    adc->setSmoothing(smoothing == 1);    
}

uint8_t SCALE::getSmoothing() {
  if (adc->getSmoothing()) {
    return 1;    
  } else {
    return 0;
  }
}


// int32_t SCALE::getTimer()
// {
//   //watch out, our main variables are unsigned!
//   int32_t timerCounter = 0;
//   if (timerMillis > 0) {
//     //timer has been stopped but has value saved
//     timerCounter = timerMillis;    
//   } else if (timerMillisRunning > 0) {
//     //timer is still running, so return our timerMillisRunning value
//     timerCounter = timerMillisRunning;
//   } else {
//     return 0;
//   }

//   //if you don't like returning negative values, uncomment the following block
// //  if (timerCounter >= timerDelay) {
//     timerCounter -= timerDelay;
// //  } else {
// //    timerCounter = 0;
// //  }
  
//   return timerCounter;
// }

// void SCALE::startStopResetTimer() {
//   //this is a simple function tha cycles all the timer statues:
//   //zeroed(reseted) -> start -> stop -> reset
//   if (timerRunning) {
//     stopTimer();
//   } else if (!timerRunning && (timerMillis > 0)) {
//     resetTimer();  
//   } else {
//     startTimer(true);
//   }  
// }

// void SCALE::startTimer(bool manual)
// {
//    Serial.print("starting timer with manual option "); Serial.println(manual);
//   manualTimer = manual; //manual timer running
//   timerMillis = 0;
//   timerStopMillis = 0;  
//   timerMillisRunning = 0;
//   timerStartMillis = millis();
//   timerRunning = true;
// }


// void SCALE::stopTimer()
// {
//    Serial.println("actually stopping timer");
//   if (timerStopMillis == 0) {
//     timerStopMillis = millis();
//   }
//   manualTimer = false;
//   if (timerStopMillis < timerStartMillis) {
//     //avoid overflow
//     timerStopMillis = timerStartMillis;
//   }
//   timerMillis = timerStopMillis - timerStartMillis; //freeze value
//   timerStartMillis = 0;
//   timerMillisRunning = 0;
//   timerRunning = false;
// }

// void SCALE::checkStopTimer()
// {
//   if (hasSettledQuick && lastUnitsRead >= timerStopWeight) {
//     stopTimer();
//   } else {
//     // do not stop yet, just keep the time
//     timerStopMillis = millis();
//     timerMillisRunning = timerStopMillis - timerStartMillis;
//   }
// }


// void SCALE::resetTimer()
// {  
//    Serial.println("resetting timer");
//   stopTimer();
//   timerMillis = 0;
//   timerStopMillis = 0;
//   manualTimer = false;
// }



// void SCALE::updateTimer()
// {
//   if (!manualTimer && autoStartTimer == 1 && timerStartMillis == 0 && timerMillis == 0 && (lastUnitsRead >= timerStartWeight)) {
//     //start timer
//     if (roc > rocStartTimer) {
//       startTimer();
//     }
//   }
//   if (!manualTimer && autoStartTimer == 1 && timerStartMillis > 0 && (lastUnitsRead < timerStartWeight)) {
//     //don't care...false positive
//      Serial.print("resetting timer because lastUnitsRead is "); Serial.println(lastUnitsRead);
//     resetTimer();
//   }
//   if (!manualTimer && autoStopTimer && timerStartMillis > 0 && roc <= rocStopTimer) {
//     //stop timer, we are done
//      Serial.println("should we stop the timer ?");
//     checkStopTimer();
//   }
//   if (timerStartMillis > 0) {
//     timerMillis = millis() - timerStartMillis;    
//   }
// }


bool  SCALE::tare(byte type, bool saveWeight, bool autoTare, bool calibrate) 
{  
   Serial.print("will tare adc with type/save "); Serial.print(type); Serial.print("/"); Serial.println(saveWeight);
  if (lastUnitsRead > 0 && saveWeight) {
    lastTareWeight = lastUnitsRead;
    lastTareWeightRounded = roundToDecimal(lastUnitsRead,decimalDigits);
  }
  
  adc->tare(type,calibrate);
  lastUnitsRead = 0;
  hasSettled = true;
  hasSettledQuick = true;  
  stableWeightCounter=65535; //we could find the proper max value as we do in readUnits, but no need to do it. Just set an arbitary huge number.
  stableWeightCounterQuick = 65535;
  lastStableWeight = 0;
  if (!autoTare) {
    autoTareUsed = false;
    // resetTimer();
    zeroTrackingUntil = millis() + 1000; //enable zeroTracking for 1s
  }
  return true;
}

double SCALE::roundToDecimal(double value, int dec)
{
  double mlt = powf( 10.0f, dec );
  value = roundf( value * mlt ) / mlt;
  return value;
}

int32_t  SCALE::readRaw(uint8_t samples){
  return adc->readRaw(samples);
}

double SCALE::readUnits(uint8_t samples)
{
  double finalUnits = 0.0;
  double units = 0.0;
  uint8_t adcSpeed = 10;
  double absUnits = 0.0;
  
  if (samples <=0) {
    samples=1;
  }
  units = adc->readUnits(samples);  
  absUnits = fabs(units);
  adcSpeed = adc->getSpeed();
  uint16_t sampleSize = adcSpeed*stableWeightSampleSizeMultiplier;
  uint16_t sampleSizeQuick = adcSpeed*stableWeightSampleSizeMultiplierQuick;
  
  if (fabs(absUnits - fabs(lastUnitsRead)) <= stableWeightDiff) {
    //ok...stable weight, increase counter and change hasSettled flag
    if (stableWeightCounterQuick < sampleSizeQuick) {
      stableWeightCounterQuick++;
    }
    if (stableWeightCounter < sampleSize) { 
      stableWeightCounter++;
    }
    if (stableWeightCounterQuick >= sampleSizeQuick) {
      if (!hasSettledQuick) {
        hasSettledQuick = true;   
      }
    }
    if (stableWeightCounter >= sampleSize) {
      if (!hasSettled) {
        //1st time
        hasSettled = true;
        lastStableWeight = units;        
      }
    }
  } else {
    //oops...
    hasSettled = false;
    hasSettledQuick = false;   
    stableWeightCounter=0;
    stableWeightCounterQuick = 0;
    lastStableWeight = units + fakeStabilityRange*2;
  }
  
  if (!calibrationMode && autoTare && !autoTareUsed && (absUnits > autoTareMinWeight)) {
    if (hasSettled) {
       Serial.println("Auto tare is enabled, taring...");      
      tare(0,true,true,false); //do a quick tare, save the weight to history, but without adc calibration
      // if (autoStartTimer == 2) {
      //   startTimer();
      // } else if (autoStartTimer ==1) {
      //   resetTimer();
      // }
      autoTareUsed = true;
      units = 0.0;
    } else {      
      //we are waiting for hasSettled to do auto tare, reset timer.
      // resetTimer();
    }
  }

  if (autoTareNegative && units < 0) {
       Serial.println("Auto tare on negative value is enabled is enabled, taring...");   
      tare(0,false,true,false);
  }
  
  finalUnits = units;
  if (fakeStabilityRange > 0) {
    if (fabs(absUnits - fabs(lastFakeRead)) <= fakeStabilityRange) {
      if ((millis() - lastFakeRefresh) < fakeDisplayLimit*1000){
        //we are fine...show the fake
        finalUnits = lastFakeRead;
      } else {
        //nope...not any more
        lastFakeRead = units;
        lastFakeRefresh = millis();
      }
    } else {
      lastFakeRead = units;
      lastFakeRefresh = millis();
    }
  }
  lastUnitsRead = units;
  if (!calibrationMode) {
    // updateTimer();
  }

  bool zTrTare = false;
  if (!calibrationMode && hasSettled && absUnits > 0 && (absUnits < zeroTracking)){
    zTrTare = true;
  } else if (zeroTrackingUntil > 0 && (millis() < zeroTrackingUntil)) {
    zTrTare = true;    
  }

  if (zTrTare) {
    Serial.print("Zero tracking is enabled, taring... units = "); Serial.println(units);
    tare(1,false,true,false);
    units = 0.0;
    finalUnits = 0.0;
  }


//  if (fabs(units) <= zeroRange) { units = 0.00; }  
  if (fabs(roc) <= zeroRange) { roc = 0.00; }
//  if (fabs(fabs(units)-fabs(lastStableWeight)) <= fakeStabilityRange) { units = lastStableWeight; }
  if (fabs(finalUnits) <= zeroRange) { finalUnits = 0.00; }
  finalUnits = roundToDecimal(finalUnits,decimalDigits);

//  if (rocInterval > 1000) {
//    rocInterval = 1000;
//  }
  
//  //moving average
//  if (historyBuffTail >= HISTORY_BUFF_LENGTH) { //overflow, should not happend -> do not allow rocInterval > 1s or increase HISTORY_BUFF_LENGTH
//    historyBuffTail = 0;
//  }
//  historyBuffer[historyBuffTail] = finalUnits;
//  historyBuffTail++;
//  uint32_t rocMillis = millis();  
////  for (int i = 0;i<HISTORY_BUFF_LENGTH;i++) {
////    coeff[i] = (1-(HISTORY_BUFF_LENGTH-i));
////  }
//  double deriv = 0.0;
////   Serial.print("deriv= ");
//  for (int i = 0;i<HISTORY_BUFF_LENGTH;i++) {
//    deriv += historyBuffer[i]*coeff[i];
////     Serial.print(deriv); Serial.print("->");
//  }
//  //1st deriv
//  deriv = deriv/12;
////   Serial.print(deriv);
//   Serial.print("     calculating RoC deriv="); Serial.println(deriv);
  


  
//  if (millis() > (rocLastCheck + rocInterval)){
//     Serial.print("calculating RoC based on sample number="); Serial.print(historyBuffTail+1); Serial.print(" // ");
//     Serial.print("divider="); Serial.println(rocMillis - rocLastCheck);
//    
//    //calculate done, store and restart
//    double movingAvg = 0;
//    if (historyBuffTail+1 > 0) {
//      movingAvg = historyBufferAdd/(historyBuffTail+1);      
//    }
//    roc = (movingAvg-lastMovingAverage)*1000/(rocMillis - rocLastCheck); 
//    rocLastCheck = rocMillis + rocInterval; 
//    historyBuffTail = 0;
//    lastMovingAverage = movingAvg;
//
//
//    
////    double movingAvg = 0;
////    if (historyBuffTail+1 > 0) {
////      movingAvg = historyBufferAdd/(historyBuffTail+1);      
////    }
////    roc = (movingAvg-lastMovingAverage)*(historyBuffTail+1)*rocInterval/(rocMillis - rocLastCheck);  
//////    roc = movingAvg*1000/(rocMillis - rocLastCheck); 
////    lastMovingAverage = movingAvg;
////    rocLastCheck = rocMillis + rocInterval; 
////    historyBuffTail = 0;
////    historyBufferAdd = 0;
////    //historyBuffer[historyBuffTail] = 0;
//  } else {
//    historyBufferAdd -= historyBuffer[historyBuffTail];
//    historyBufferAdd += finalUnits;
//    historyBuffer[historyBuffTail] = finalUnits;
//    historyBuffTail++;    
//  }
  


  
  uint32_t rocMillis = millis();  
//  if (rocMillis > rocLastCheck + rocInterval) {
    //todo: something more sophisticated
//    double movingAvg = historyBufferAdd/HISTORY_BUFF_LENGTH;
    roc = (finalUnits-lastUnits)*1000/(rocMillis - rocLastCheck);  
    rocLastCheck = rocMillis;
//    lastMovingAverage = movingAvg;
//    rocLastCheck = rocMillis + rocInterval;
//  }
  
  lastUnits = finalUnits;
  return finalUnits;
}





void SCALE::calibrateADC() 
{  
  adc->calibrateADC();
}






void SCALE::calibrate(float targetWeight, int maxMillis, float targetDiff) 
{  
  calibrationMode = true;
   Serial.println("calibrating...");
  for (uint8_t i=0;i<5;i++){
    tare(0,false,true,true);
    delay(100);
  }
  uint32_t calibrationStartTime = millis();
  float weight = 0.0;
  float oldstableWeightDiff = stableWeightDiff;
  //increase stableWeightDiff to 1g  
  stableWeightDiff = 1.0;
  //loop until settled
  uint32_t elapsedTime = millis() - calibrationStartTime;  
  while (!hasSettled || abs(weight) < 10.0) {
    weight = readUnits(1);   
    Serial.println("weight:" + String(weight)); 
    elapsedTime = millis() - calibrationStartTime;
    if (elapsedTime > maxMillis) {
       Serial.println("calibration failed");
      return;
    }
  }
  
   Serial.println("calibration stage 1");
  float switchModeThreshold = targetWeight*0.05; //5%
  bool initAutoCalibrationComplete = false;
  bool fineTuneAutoCalibrationComplete = false;
  bool slowTuneAutoCalibrationComplete = false;
  uint8_t oldSpeed = adc->getSpeed();
  bool oldSmoothing = adc->getSmoothing();
  adc->setSmoothing(false);  
  adc->setSpeed(80);
  //let's get close enough very fast
  float newCalFactor = adc->calFactor;
  while (!initAutoCalibrationComplete && elapsedTime<maxMillis) {
    weight = readUnits(1);
    if ( fabs(weight-targetWeight) <= switchModeThreshold) {
      //continue in low speed
      initAutoCalibrationComplete = true;
    } else {
      if (weight > targetWeight) {
        //increase calfactor
        newCalFactor = newCalFactor + 50;
      } else {
        //decrease calfactor
        newCalFactor = newCalFactor - 50;
      }    
      adc->setCalFactor(newCalFactor);
       Serial.print("new calfactor/weight "); Serial.print(newCalFactor); Serial.print("/"); Serial.println(weight);
    }
    elapsedTime = millis() - calibrationStartTime;  
  }
   Serial.println("stage 1 completed...");
  switchModeThreshold = targetWeight*0.01; //1%
  
   Serial.println("calibration stage 2");
  while (!fineTuneAutoCalibrationComplete && elapsedTime<maxMillis) {
    weight = readUnits(1);
    if ( fabs(weight-targetWeight) <= switchModeThreshold) {
      fineTuneAutoCalibrationComplete = true;
    } else {
      if (weight > targetWeight) {
        //increase calfactor
        newCalFactor = newCalFactor + 1;
      } else {
        //decrease calfactor
        newCalFactor = newCalFactor - 1;
      }    
      adc->setCalFactor(newCalFactor);
       Serial.print("new calfactor/weight "); Serial.print(newCalFactor); Serial.print("/"); Serial.println(weight);
    }
    elapsedTime = millis() - calibrationStartTime;  
  }
  stableWeightDiff = oldstableWeightDiff;
   Serial.println("stage 2 completed...");

  bool above = false;
  bool below = false;
  //finally, switch to 10SPS and do the final approach
   Serial.println("calibration stage 3");
   Serial.println("swithing to 10SPS");
  adc->setSpeed(10);
  bool resetStableWeightCounter = true;
  while (!slowTuneAutoCalibrationComplete && elapsedTime<maxMillis) {
    weight = readUnits(1);
    if ( (fabs(weight-targetWeight) <= targetDiff) && above && below) {
      //almost done...wait until settled
      if (resetStableWeightCounter) {
        hasSettled = false;
        resetStableWeightCounter = false;
        stableWeightCounter = 0;
         Serial.println("Almost there...reseting hasSettled status");
      }
      if (hasSettled) { slowTuneAutoCalibrationComplete = true; }
    } else {
      if (weight > targetWeight) {
        //increase calfactor
        above = true;
        newCalFactor = newCalFactor + finetuneCalibrationAdj;
      } else {
        //decrease calfactor
        below=true;
        newCalFactor = newCalFactor - finetuneCalibrationAdj;
      }    
      adc->setCalFactor(newCalFactor);
       Serial.print("new calfactor/weight "); Serial.print(newCalFactor); Serial.print("/"); Serial.println(weight);
    }
    elapsedTime = millis() - calibrationStartTime;  
  }

  if (elapsedTime>=maxMillis){
     Serial.println("calibration timed out...please increase time");
  } else {
     Serial.println("final calibration completed...");
  }
  adc->setSpeed(oldSpeed);  
  adc->setSmoothing(oldSmoothing);  
  calibrationMode = false;
  autoTareUsed = true;
   Serial.println("DONE");
}

void SCALE::setCalFactor(float calFactor)
{
  adc->setCalFactor(calFactor);
}


float SCALE::getCalFactor()
{
  return(adc->calFactor);
}
