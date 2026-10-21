
// Example sketch which shows how to display some patterns
// on a 64x32 LED matrix
//

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// #include <C:/Users/adaocong/LDV/LED_SEEV/cam_vao.h>
// #include <DUOC_VAO.h>
#include <CHU_TO.h>
#include "ota_update.h"
#include <C:/Users/adaocong/LDV/LED_SEEV/Fonts/kongtext4pt7b.h>

#define DEFAULT_WIFI_OTA    "Linh Anh"
#define DEFAULT_PASS_OTA    "08032025"
#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another
#define GREEN_SIG 32
#define RED_SIG 33
//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK, myWHITE, myRED, myGREEN, myBLUE;

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

void drawText_Green(int colorWheelOffset)
{
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
  dma_display->println("400;");
}

void drawText_Red(int colorWheelOffset)
{
  
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
  dma_display->println("400;");
}
void drawText_OTA_connecting()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(7, 0);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(DEFAULT_WIFI_OTA);
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(15, 0);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(DEFAULT_PASS_OTA);

  dma_display->setCursor(23, 0);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println("VERSION:"); 

  dma_display->setCursor(31, 0);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myRED);
  dma_display->println(FW_VERSION);
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

  if ((digitalRead(GREEN_SIG) == LOW) or (digitalRead(RED_SIG))
  {
    WiFi.begin(DEFAULT_WIFI_OTA, DEFAULT_PASS_OTA);
    Serial.print(DEFAULT_WIFI_OTA);
    Serial.print("-");
    Serial.println(DEFAULT_PASS_OTA);
    while (WiFi.status() != WL_CONNECTED) 
    {
      Serial.print(".");
      delay(50);
      drawText_OTA_connecting()
    }
    dma_display->clearScreen();
    checkAndUpdateFirmware();  // gọi hàm OTA
    while(1);
  }
}

uint8_t wheelval = 0;
void loop() {

    // animate by going through the colour wheel for the first two lines
    drawText(wheelval); 
    delay(20);
  
}
