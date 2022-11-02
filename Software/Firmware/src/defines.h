#define ADC_LDO_EN_PIN 21
#define ADC_LDO_ENABLE 1 //high or low for enable ??? Check datasheet of your LDO.
#define ADC_LDO_DISABLE 0 //high or low for disable ??? Check datasheet of your LDO.
  
#define ADC_PDWN_PIN 5
#define ADC_DOUT_PIN 19
#define ADC_SCLK_PIN 18
#define ADC_GAIN0_PIN 16
#define ADC_GAIN1_PIN 17
#define ADC_SPEED_PIN 22
#define ADC_A0_PIN 0 //tied to GND AIN1 in PRO
#define ADC_TEMP_PIN 0 //tied to GND AIN1 in PRO

#define DISPLAY_WIDTH 128 // display width, in pixels
#define DISPLAY_HEIGHT 32
#define DISPLAY_MOSI_PIN 13 //SDA if using I2C
#define DISPLAY_CLK_PIN 14 //SCL is using I2C

#define INITIALISING_SECS 5 //high or low for enable ??? Check datasheet of your LDO.

#define  POWER_BUTTON_PIN 32
#define  TARE_BUTTON_PIN 33
#define  SECONDARY1_BUTTON_PIN  12



#define POWERLATCH 35
#define LED 2
#define POWERDOWNTIMER 100


uint8_t batReadInterval = 30; //in seconds - defines how frequently we should read the voltage from our voltage divider.
float vinVoltage = 0.0;
String resolutionLevel = "";
uint32_t lastActionMillis = 0; 
bool snooze = false;
bool lightSleep = false;
bool wakeup = false;
bool calibrationMode = false;
bool initialising = false;
uint32_t beginTare = 0; //0 = do not tare, >0 tare when millis() = beginTare
byte tareType = 0;
bool calibrateTare = false;
bool saveWeight = false;
bool calibrating = false;
float calibrateToUnits = 0.0;
double gramsDbl = 0.0;
String gramsStr = "";
String timerStr = "";
float timerInSeconds=0.0;
bool deepSleepNow = false;
uint8_t graphInSection = 0;
bool resetWeightGraph = false;
