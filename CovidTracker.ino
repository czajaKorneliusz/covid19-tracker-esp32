#pragma region oled
#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN 0x10
#endif

#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23

#define TFT_BL 4 // Display backlight control pin
#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

#pragma endregion


#define TIME_UNTIL_NEXT_SCAN 600000000 //in microseconds

#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <Button2.h>
#include <HTTPClient.h>


const char* ssid = "yourNetwork";
const char* password = "secretPassword";


const String country = "Poland";//Country of intrest

#define ALERT_PIN 33

HTTPClient http;

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

RTC_DATA_ATTR int casesInCountry;


void ConnectToWifi()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
}

boolean isValidNumber(String str) {
  for (byte i = 0; i < str.length(); i++)
  {
    if (isDigit(str.charAt(i))) return true;
  }
  return false;
}

void setupDisplay()
{

  tft.init();
  if (TFT_BL > 0)
  { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
    pinMode(TFT_BL, OUTPUT);                // Set backlight pin to output mode
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(4);
}

void displayInfo(int count) {
  setupDisplay();
  tft.setCursor(0, 0);
  tft.fillScreen(TFT_BLACK);
  tft.println(country);
  tft.println(count);
  delay(15000);
}

int GetRequestFromSite()
{
  if ((WiFi.status() == WL_CONNECTED))
  { //Check the current connection status
    http.begin("https://www.worldometers.info/coronavirus/"); //Specify the URL
    int httpCode = http.GET();                                //Make the request

    if (httpCode == 200)
    { //Check for the returning code
      uint8_t buff[128] = { 0 };

      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();

      String twoLastStrings[2];// = (char *)buff;
      // read all data from server
      while (http.connected() ) {
        // get available data size
        size_t size = stream->available();

        if (size) {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff) - 1) ? sizeof(buff) - 1 : size));
          twoLastStrings[0] = "";
          twoLastStrings[0] = twoLastStrings[1];
          twoLastStrings[1] = "";
          twoLastStrings[1] = (char *)buff;

          String tempString = "";
          tempString.concat(twoLastStrings[0]);
          tempString.concat(twoLastStrings[1]);
          int place = tempString.indexOf(country);
          if (place != -1) {

            place = place - country.length();

            delay(10);
            stream->readBytes(buff, ((size > sizeof(buff) - 1) ? sizeof(buff) - 1 : size));
            tempString.concat((char *)buff);
            String myString = tempString;
            myString.concat((char *)buff);
            myString = myString.substring(myString.indexOf(country));
            String result = myString.substring(myString.indexOf("\">") + 2);
            String cases = result.substring(0, (result.indexOf("</")));
            if (
              isValidNumber(cases) && cases.toInt() != 0)
            {
              http.end();
              return cases.toInt();
            }
            else {
              http.end();
              return -1;
            }
          }
        }
        delay(1);
      }
    }
    else
    {
      Serial.print("Error on HTTP request ");
      Serial.println(httpCode);


    }

    http.end(); //Free the resources
  }
  return -1;
}


void alarm() {
  digitalWrite(ALERT_PIN, HIGH); // sets the digital pin on
  delay(1500);            // waits for a second
  digitalWrite(ALERT_PIN, LOW);  // sets the digital pin off
  delay(1000);
}



void setup () {
  //button_init();
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    displayInfo(casesInCountry);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    esp_sleep_enable_timer_wakeup(TIME_UNTIL_NEXT_SCAN);
    esp_deep_sleep_start();
  }

  ConnectToWifi();
  pinMode(ALERT_PIN, OUTPUT);

  int temp = -1;
  while (temp == -1) {
    temp = GetRequestFromSite();
    Serial.println(temp);
    if (casesInCountry != temp && temp != -1)
    {
      casesInCountry = temp;
      alarm();
      displayInfo(temp);
      delay(1000);
    }
  }




  esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
  esp_sleep_enable_timer_wakeup(TIME_UNTIL_NEXT_SCAN);
  esp_deep_sleep_start();

}


void loop() {
  //unreachable code
}
