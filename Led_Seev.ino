
// Example sketch which shows how to display some patterns
// on a 64x32 LED matrix
//

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// #include <C:/Users/adaocong/LDV/LED_SEEV/cam_vao.h>
// #include <DUOC_VAO.h>
#include <CHU_TO.h>
#include "ota_update.h"
#include <C:/Users/adaocong/LDV/LED_SEEV/Fonts/kongtext4pt7b.h>

#define DEFAULT_WIFI_OTA    "LDV_Inno"
#define DEFAULT_PASS_OTA    "1NNovation!"
#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another
#define GREEN_SIG 32
#define RED_SIG 33
//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;
int GreenStatus = 0;
int RedStatus = 0;
uint16_t myBLACK, myWHITE, myRED, myGREEN, myBLUE;
uint32_t GreenStartTime = 0;
uint32_t RedStartTime = 0;
int GreenMaxTime = 400;
int RedMaxTime = 400;
int Green_old_time = 0;
int Red_old_time = 0;
String result_ota_update = "";
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void drawText_Green()
{
  uint32_t hientai = (millis() - GreenStartTime) / 1000;
  int32_t thoi_gian_con_lai = GreenMaxTime - hientai;
  if(Green_old_time != thoi_gian_con_lai)
  {
    dma_display->clearScreen();
    Green_old_time = thoi_gian_con_lai;
  }
  if (thoi_gian_con_lai < 0)
  {
    thoi_gian_con_lai = 0;
  }
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&CHU_TO);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
  dma_display->setCursor(2, 13);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(0, 255, 0));
  dma_display->println('D');
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(5, 27);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(0, 255, 0));
  dma_display->println('E');
  dma_display->setCursor(18, 31);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(0, 255, 0));
  dma_display->print(thoi_gian_con_lai);
  dma_display->println(';');
}

void drawText_Red()
{
  uint32_t hientai = (millis() - RedStartTime) / 1000;
  int32_t thoi_gian_con_lai = RedMaxTime - hientai;
  if(Red_old_time != thoi_gian_con_lai)
  {
    dma_display->clearScreen();
    Red_old_time = thoi_gian_con_lai;
  }
  if (thoi_gian_con_lai < 0)
  {
    thoi_gian_con_lai = 0;
  }
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&CHU_TO);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(10, 13);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(255, 0, 0));
  dma_display->println('C');
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(5, 27);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(255, 0, 0));
  dma_display->println('E');

  dma_display->setCursor(18, 31);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(255, 0, 0));
  dma_display->print(thoi_gian_con_lai);
  dma_display->println(';');
}
void drawText_OTA_connecting()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(2, 7);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(DEFAULT_WIFI_OTA);
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(2, 15);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(DEFAULT_PASS_OTA);

  dma_display->setCursor(2, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println("VER:"); 

  dma_display->setCursor(35, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(FW_VERSION);
}

void drawText_OTA_updating()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(2, 7);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_WIFI_OTA);
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(2, 15);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_PASS_OTA);

  dma_display->setCursor(2, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println("UPDATING"); 
}

void drawText_OTA_failed()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(2, 7);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(DEFAULT_WIFI_OTA);
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(2, 15);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(DEFAULT_PASS_OTA);

  dma_display->setCursor(2, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println("UPDATE FAILED"); 
}

void drawText_OTA_Successfully()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(2, 7);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_WIFI_OTA);
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(2, 15);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_PASS_OTA);

  dma_display->setCursor(2, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(result_ota_update); 
}

void setup() {

  // Module configuration
  Serial.begin(115200);
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );
  pinMode(GREEN_SIG, INPUT_PULLUP);
  pinMode(RED_SIG, INPUT_PULLUP);
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(150); //0-255
  dma_display->clearScreen();
  myBLACK = dma_display->color565(0, 0, 0);
  myWHITE = dma_display->color565(255, 255, 255);
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);

  if ((digitalRead(GREEN_SIG) == LOW) or (digitalRead(RED_SIG)==LOW))
  {
    delay(100);
    if ((digitalRead(GREEN_SIG) == LOW) or (digitalRead(RED_SIG)==LOW))
    {
      WiFi.begin(DEFAULT_WIFI_OTA, DEFAULT_PASS_OTA);
      // Serial.print(DEFAULT_WIFI_OTA);
      // Serial.print("-");
      // Serial.println(DEFAULT_PASS_OTA);
      while (WiFi.status() != WL_CONNECTED) 
      {
        // Serial.print(".");
        delay(50);
        drawText_OTA_connecting();
      }
      dma_display->clearScreen();
      drawText_OTA_updating();
      result_ota_update = checkAndUpdateFirmware();  // gọi hàm OTA
      dma_display->clearScreen();
      drawText_OTA_Successfully();
      while(1);
    }

  }
}

uint8_t wheelval = 0;
void loop() {
    if((digitalRead(GREEN_SIG)==LOW) && (GreenStatus == 0) && (digitalRead(RED_SIG)==HIGH))
    {
      delay(10);
      if((digitalRead(GREEN_SIG)==LOW) && (GreenStatus == 0) && (digitalRead(RED_SIG)==HIGH))
        {
          GreenStartTime = millis();
          GreenStatus = 1;
        }
    }

    if((digitalRead(RED_SIG)==LOW) && (RedStatus == 0) && (digitalRead(GREEN_SIG)==HIGH))
    {
      delay(10);
      if((digitalRead(RED_SIG)==LOW) && (RedStatus == 0) && (digitalRead(GREEN_SIG)==HIGH))
      {
        RedStartTime = millis();
        RedStatus = 1;
      }

    }

    if((digitalRead(RED_SIG)==LOW) && (digitalRead(GREEN_SIG)==LOW))
    {
      RedStartTime = 0;
      GreenStartTime = 0;
      GreenStatus = 0;
      RedStatus = 0;
    }

    if((digitalRead(GREEN_SIG)==HIGH) && (GreenStatus == 1))
    {
      delay(10);
      if((digitalRead(GREEN_SIG)==HIGH) && (GreenStatus == 1))
      {
        GreenStartTime = 0;
        GreenStatus = 0;
      }
    }

    if((digitalRead(RED_SIG)==HIGH) && (RedStatus == 1))
    {
      delay(10);
      if((digitalRead(RED_SIG)==HIGH) && (RedStatus == 1))
      {
        RedStartTime = 0;
        RedStatus = 0;
      }
    }
    if (GreenStatus == 1)
    {
      drawText_Green();
    }
    if (RedStatus == 1)
    {
      drawText_Red();
    }
    if((GreenStatus == 0) && (RedStatus == 0))
    {
      dma_display->clearScreen();
    } 
}
