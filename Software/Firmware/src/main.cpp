#include <Arduino.h>
#include "esp32Helper.h"
#include <Wire.h>
#include <Ticker.h>

#include <Adafruit_GFX.h>
#include "FreeSansBold9pt7b.h"
#include "FreeSansBold12pt7b.h"
#include <Adafruit_SSD1306.h>
#include "defines.h"
#include "scale.h"
#define ADS1232
#include "MONITORING.h"
#include "ArduinoNvs.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "credentials.h"
#include "udp_logging.h"
#include "esp_log.h"

AsyncWebServer server(80);
// const char* ssid = SSID;
// const char* password = PASSWORD;

SCALE scale = SCALE(ADC_PDWN_PIN, ADC_SCLK_PIN, ADC_DOUT_PIN, ADC_A0_PIN, ADC_SPEED_PIN, ADC_GAIN1_PIN, ADC_GAIN0_PIN, ADC_TEMP_PIN);
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);
Ticker powerTicker;
Ticker displayTicker;
uint32_t powertimer = POWERDOWNTIMER;
MONITORING monitoring = MONITORING(23, 36); // enable mosfet pin = 23, vin on gpio36
bool timermode = false;
uint32_t start_timer = 0;
uint32_t calFactorULong = CALFACTORDEFAULT;
bool do_calibration = false;


#ifdef WIFIDEBUG
bool activeWifi = 1;
#else
bool activeWifi = 0;
#endif

uint32_t calibrationTimoutTime = 120000;
bool updateRunning = false;

double currentScaleValue=0;
double lastScaleValue=0;

#include "mybuttons.h"

int count = 26;
void fct_bootLogo()
{
  display.clearDisplay();
  int ico_width = 32;
  int ico_height = 32;
  display.drawBitmap(int((DISPLAY_WIDTH - ico_width) / 2), int((DISPLAY_HEIGHT - ico_height) / 2), coffee[count], ico_width, ico_height, WHITE);

  display.display();
  count--;
  if (count < 0)
    count = 27;
}

void fct_wifiLogo()
{
  display.clearDisplay();
  // display count animation image
  int ico_width = 32;
  int ico_height = 32;
  display.drawBitmap(int((DISPLAY_WIDTH - ico_width) / 2), int((DISPLAY_HEIGHT - ico_height) / 2), wifiAnimation[count], ico_width, ico_height, WHITE);
  display.display();
  count--;
  if (count < 0)
    count = 27;
}

float fct_roundToDecimal(double value, int dec)
{
  double mlt = powf(10.0f, dec);
  value = roundf(value * mlt) / mlt;
  return (float)value;
}

void fct_powerDown()
{
  NVS.close();
  digitalWrite(POWERLATCH, LOW);
  digitalWrite(LED, LOW);
}

void fct_powerResetTimer(int t)
{
  powertimer = t;
}

void fct_powerResetTimer()
{
  fct_powerResetTimer(POWERDOWNTIMER);
}

void fct_powerDownTicker()
{
  if(abs(currentScaleValue-lastScaleValue)>MINWEIGHTCHANGETOPREVENTSLEEP){
    fct_powerResetTimer();
  }
  lastScaleValue=currentScaleValue;

  powertimer--;
  ESP_LOGV("main", "power down timer %d",powertimer);

  if (powertimer < 1)
  {
    ESP_LOGI("main", "power down");
    fct_powerDown();
  }
}

void fct_timerMode()
{
  start_timer = millis();
  timermode = !timermode;
}

void fct_powerUp()
{
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
  uint32_t minimum = (uint32_t)(value * (1 - range));
  uint32_t maximum = (uint32_t)(value * (1 + range));
  return ((val >= minimum) && (val <= maximum));
}

void fct_initScale()
{
  pinMode(ADC_LDO_EN_PIN, OUTPUT);
  digitalWrite(ADC_LDO_EN_PIN, ADC_LDO_ENABLE);


  const uint8_t DEFAULT_ADC_SPEED = 10; // 10 or 80
  //--------
  scale.begin(0, 128, DEFAULT_ADC_SPEED);
  scale.calibrateADC();

  uint32_t _calFactorULong_ = NVS.getInt("calFactorULong");
  // set CalcFactor only if in a plausible range
  if (inRange(_calFactorULong_, NVS.getInt("validitycheck"), 0.05))
  {
    calFactorULong = _calFactorULong_;
    scale.setCalFactor(calFactorULong / 10000.0);
    ESP_LOGI("main", "setCalFactor: valid");
  }
  else
  {
    ESP_LOGI("main", "setCalFactor: not valid");
  }

//  scale.setSensitivity(120);
//  scale.setSmoothing(5);
  
 scale.setSensitivity(255);
 scale.setSmoothing(1);
 scale.setSpeed(DEFAULT_ADC_SPEED);
}

charging getSOCIdx()
{
  if (soc_battery < 10)
  {
    return BAT_EMPTY;
  }
  else if (soc_battery < 76)
  {
    return BAT_50;
  }
  else if (soc_battery < 101)
  {
    return BAT_100;
  }
  else
  {
    return BAT_EMPTY;
  }
  return BAT_EMPTY;
}

void fct_showText(String text, String text2)
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setFont(&FreeSansBold12pt7b);
  display.setTextSize(1);

  int16_t x = 0;
  int16_t y = 32;
  int16_t x1 = 0;
  int16_t y1 = 0;
  u_int16_t th = 0;
  u_int16_t tw = 0;
  //------------------------
  // for right alignement
  display.getTextBounds(text, x, y, &x1, &y1, &tw, &th);
  display.setCursor(DISPLAY_WIDTH - tw - 10, 20);
  display.println(text);

  if (timermode)
  {
    display.fillRect(0, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH, DISPLAY_HEIGHT, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setFont(&FreeSansBold12pt7b);
    display.setTextSize(1);
    display.getTextBounds(text2, x, y, &x1, &y1, &tw, &th);
    display.setCursor(DISPLAY_WIDTH - tw - 10, DISPLAY_HEIGHT - 9);
  }
  else
  {
    display.setFont(&FreeSansBold9pt7b);
    display.setTextSize(1);
    display.setCursor(5, DISPLAY_HEIGHT - 4);
    display.drawBitmap(int((128 - 25)), DISPLAY_HEIGHT - 18, battery[getSOCIdx()], 16, 16, SSD1306_WHITE);
  }
  display.println(text2);
  display.display(); // Show initial text
  display.setTextColor(SSD1306_WHITE);
  delay(100);
}

void fct_calibrationDisplay()
{
  switch (scale.getCalibrationStatus())
  {
  case calibrationStatus::ERROR:
    fct_showText("ERROR!.", "Timeout -> Retry");
    calibrationTimoutTime = 360000;
    break;
  case calibrationStatus::START:
    fct_showText("Calibration", "no weight!");
    break;
  case calibrationStatus::PLACE:
    fct_showText("Calib. 1/4", "place 100g");
    break;
  case calibrationStatus::STAGE_1:
    fct_showText("Calib. 2/4", "");
    break;
  case calibrationStatus::STAGE_2:
    fct_showText("Calib. 3/4", "");
    break;
  case calibrationStatus::STAGE_3:
    fct_showText("Calib. 4/4", "");
    break;
  case calibrationStatus::FINISHED:
    fct_showText("Calib. Done", "Finished");
    break;
  default:
    break;
  }
}

void fct_callCalibrateScale()
{
  do_calibration = true;
}

void fct_calibrateScale()
{
  timermode = false;
  ESP_LOGD("main", "Start Calibration");
  for (int i = 0; i < 20; i++)
  {
    delay(20);
    scale.readUnits(1);
  }
  scale.calibrate(CALIBRATIONWEIGHT, calibrationTimoutTime, 0.05);
  if (scale.getCalibrationStatus() == calibrationStatus::FINISHED)
  {
    calFactorULong = (uint32_t)(scale.getCalFactor() * 10000.0);
    NVS.setInt("calFactorULong", calFactorULong);
    NVS.setInt("validitycheck", calFactorULong);
    ESP_LOGD("main", "Calibration Done:: calFactorULong: %d", calFactorULong);
  }

  for (int i = 0; i < 20; i++)
  {
    delay(100);
  }
  do_calibration = false;
}

// t is time in seconds = millis()/1000;
char *TimeToString(unsigned long t)
{
  static char str[12];

  unsigned long allSeconds = t / 1000;
  unsigned long runMillis = t % 1000;

  sprintf(str, "%d.%03d\'\'", allSeconds, runMillis);
  return str;
}

int once_perSeconde=10;
void fct_mainDisplay()
{
  once_perSeconde--;
  if(once_perSeconde<0){
    fct_powerDownTicker();
    once_perSeconde=10;
  }

  // wenn calibration
  if (do_calibration)
  {
    fct_calibrationDisplay();
    return;
  }

  // if update active
  if (activeWifi && Update.isRunning())
  {
    double prog = Update.progress() / Update.size();
    fct_showText("FW upgr.", ":: => " + String(prog) + "%");
    return;
  }

  //if timermode
  if (timermode)
  {
    fct_showText(String(fct_roundToDecimal(currentScaleValue, 2)) + " g", String(TimeToString(millis() - start_timer)));
    return;
  }

  //if scale only
  if (!timermode)
  {
    fct_showText(String(fct_roundToDecimal(currentScaleValue, 2)) + " g", "Bat: " + String(soc_battery) + "%");
    return;
  }
  return;
}

void fct_setupDisplay()
{
  Wire.begin(DISPLAY_MOSI_PIN, DISPLAY_CLK_PIN);
  ESP_LOGV("main", "Wire Init");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    ESP_LOGE("main", "SSD1306 allocation failed");
  }
  display.display();
  display.setRotation(2);
  display.clearDisplay();
}

void fct_setWifi()
{

  ESP_LOGV("main", "activeWifi to: %d", activeWifi);
  if (activeWifi)
  {
    displayTicker.detach();
    displayTicker.attach_ms(40, fct_wifiLogo);
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      ESP_LOGV("main", "Wait for connection");
    }

    ESP_LOGI("main", "Setup UDP to Server: %s & PORT: %d", UDP_SERVER_IP, UDP_SERVER_PORT);

    IPAddress udpIP;
    udpIP.fromString(UDP_SERVER_IP);
    udp_logging_init(udpIP, UDP_SERVER_PORT, udp_logging_vprintf);

    ESP_LOGV("main", "Connected to: %s", SSID);
    ESP_LOGV("main", "IP address:  %s", WiFi.localIP().toString().c_str());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Hi! I am a Scale."); });

    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
    ESP_LOGV("main", "HTTP server started");
    displayTicker.detach();
  }
}

void setup()
{
  fct_powerUp();
  Serial.begin(115200);

  fct_setupDisplay();
#ifdef WIFIDEBUG // if WiFi is enabled by default start it. otherwise after scale if requested. Good for debugging
  esp_log_level_set("*", CORE_DEBUG_LEVEL);
#endif
  ESP_LOGV("main", "Setup: fct_setupDisplay");
//---------------------
#ifdef WIFIDEBUG // if WiFi is enabled by default start it. otherwise after scale if requested. Good for debugging
  fct_setWifi();
#endif
  //---------------------
  displayTicker.attach_ms(40, fct_bootLogo);
  button_setup();
  fct_powerResetTimer();
  activeWifi = (activeWifi || wifiActivationRequested());

  // ---------------------
  NVS.begin();
  // ---------------------
  fct_initScale();
  ESP_LOGV("main", "Setup: fct_initScale");
  //---------------------

  scale.tare(2, false, true, true);
  ESP_LOGV("main", "Setup: scale.tare");
  activeWifi = (activeWifi || wifiActivationRequested());

  //---------------------
  for (int i = 0; i < 3; i++)
  {
    soc_battery = monitoring.getSOC();
    delay(30);
  }

  //---------------------
  displayTicker.detach();
//---------------------
#ifndef WIFIDEBUG
  fct_setWifi();
#endif
  //---------------------
  //main display Ticker function
  displayTicker.attach_ms(100, fct_mainDisplay);
}

uint32_t u_time = 0;
uint32_t lastVinRead = 0;

void loop()
{
  //---- Button  Loop
  button_loop();

  //---- Scale  Loop
  if ((millis() - u_time >= 30))
  {
    currentScaleValue=scale.readUnits(1);
    u_time = millis();
  }

  //---- Battery Loop
  if (millis() - lastVinRead >= (batReadInterval * 1000))
  {
    soc_battery = monitoring.getSOC();
    lastVinRead = millis();
  }


  //---- Call Calibration if requested
  if (do_calibration)
  {
    fct_calibrateScale();
  }

}