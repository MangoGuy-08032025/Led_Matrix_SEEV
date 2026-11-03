
// Example sketch which shows how to display some patterns
// on a 64x32 LED matrix
//

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// #include <C:/Users/adaocong/LDV/LED_SEEV/cam_vao.h>
// #include <DUOC_VAO.h>
#include <CHU_TO.h>
#include "ota_update.h"
#include <WebServer.h>
#include <EEPROM.h>
#include <C:/Users/adaocong/LDV/LED_SEEV/Fonts/kongtext4pt7b.h>
#define EEPROM_SIZE 512

WebServer server(80);

#define DEFAULT_WIFI_OTA    "Linh Anh"
#define DEFAULT_PASS_OTA    "08032025"
#define DEFAULT_SETUP_WIFI    "LED SETUP"
#define DEFAULT_SETUP_PASS    "12345678"
#define DEFAULT_IP    "192.168.4.1"
#define PANEL_RES_X 64      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another
#define GREEN_SIG 35
#define RED_SIG 34
#define SETUP_SIG 36
int setup_mode = 0;
//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;
int GreenStatus = 0;
int RedStatus = 0;
String Wifi = "Linh Anh";
String Password = "08032025";
uint16_t myBLACK, myWHITE, myRED, myGREEN, myBLUE;
uint32_t SetupStartTime = 0;
uint32_t GreenStartTime = 0;
uint32_t RedStartTime = 0;
int Thoi_Gian_Sang_Xanh = 400;
int Thoi_Gian_Sang_Do = 400;
int Do_Sang_Xanh = 200;
int Do_Sang_Do = 200;
int Green_old_time = 0;
int Red_old_time = 0;
String result_ota_update = "";
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8

struct Config
{
  uint16_t signature;

  uint8_t Green_brightness;
  uint8_t Red_brightness;
  int XG1;
  int YG1;

  int XG2;
  int YG2;

  int XG3;
  int YG3;

  int MAX_G;

  int XR1;
  int YR1;

  int XR2;
  int YR2;

  int XR3;
  int YR3;

  int MAX_R;
};

Config cfg;

//====================================================
// DEFAULT VALUES
//====================================================

void setDefaultConfig()
{
  cfg.signature = 0x55AA;

  cfg.Green_brightness = 120;
  cfg.Red_brightness = 120;
  cfg.XG1 = 2;
  cfg.YG1 = 13;

  cfg.XG2 = 5;
  cfg.YG2 = 27;

  cfg.XG3 = 18;
  cfg.YG3 = 31;

  cfg.MAX_G = 1000;

  cfg.XR1 = 10;
  cfg.YR1 = 13;

  cfg.XR2 = 5;
  cfg.YR2 = 27;

  cfg.XR3 = 18;
  cfg.YR3 = 31;

  cfg.MAX_R = 2000;
}

//====================================================
// EEPROM
//====================================================

void saveConfig()
{
  EEPROM.put(0, cfg);
  EEPROM.commit();

  Serial.println("Config Saved");
}

void loadConfig()
{
  EEPROM.get(0, cfg);

  if (cfg.signature != 0x55AA)
  {
    Serial.println("First Startup");

    setDefaultConfig();

    saveConfig();
  }
  else
  {
    Serial.println("Config Loaded");
  }
}

//====================================================
// HTML
//====================================================

String makePage()
{
    String html;

    html += R"rawliteral(

<!DOCTYPE html>
<html>
<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<style>

body{
    font-family:Arial;
    padding:15px;
    background:#f5f5f5;
}

.container{
    max-width:500px;
    margin:auto;
    background:white;
    padding:15px;
    border-radius:10px;
    box-shadow:0 0 10px rgba(0,0,0,0.15);
}

h2{
    text-align:center;
    margin-bottom:20px;
}

.row{
    display:flex;
    align-items:center;
    margin-bottom:10px;
}

.row label{
    width:50px;
    font-weight:bold;
}

.row input[type=number]{
    width:90px;
    padding:6px;
    margin-right:12px;
    font-size:16px;
}

.sliderRow{
    margin-bottom:20px;
}

.sliderRow label{
    font-weight:bold;
}

.sliderRow input[type=range]{
    width:100%;
}

button{
    width:100%;
    padding:12px;
    margin-top:10px;
    font-size:18px;
    border:none;
    border-radius:6px;
    cursor:pointer;
}

.save{
    background:#27ae60;
    color:white;
}

.default{
    background:#f39c12;
    color:white;
}

.restart{
    background:#e74c3c;
    color:white;
}

</style>

</head>

<body>

<div class="container">

<h2>LED CONFIG</h2>

<form action="/save" method="POST">

<div class="sliderRow">

<label>Green Brightness:
    <span id="Green_brightVal">
)rawliteral";

    html += String(cfg.Green_brightness);

    html += R"rawliteral(
    </span>
</label>

<input
    type="range"
    min="1"
    max="255"
    step="1"
    name="Green_brightness"
    value=")rawliteral";

    html += String(cfg.Green_brightness);

    html += R"rawliteral("
    oninput="document.getElementById('Green_brightVal').innerHTML=this.value">

</div>

<div class="row">
<label>XG1</label>
<input type="number" step="1" name="xg1" value=")rawliteral";

    html += String(cfg.XG1);

    html += R"rawliteral(">

<label>YG1</label>
<input type="number" step="1" name="yg1" value=")rawliteral";

    html += String(cfg.YG1);

    html += R"rawliteral(">
</div>

<div class="row">
<label>XG2</label>
<input type="number" step="1" name="xg2" value=")rawliteral";

    html += String(cfg.XG2);

    html += R"rawliteral(">

<label>YG2</label>
<input type="number" step="1" name="yg2" value=")rawliteral";

    html += String(cfg.YG2);

    html += R"rawliteral(">
</div>

<div class="row">
<label>XG3</label>
<input type="number" step="1" name="xg3" value=")rawliteral";

    html += String(cfg.XG3);

    html += R"rawliteral(">

<label>YG3</label>
<input type="number" step="1" name="yg3" value=")rawliteral";

    html += String(cfg.YG3);

    html += R"rawliteral(">
</div>

<div class="row">
<label>GREEN MAX TIME:</label>
<input type="number" step="1" name="maxg" value=")rawliteral";

    html += String(cfg.MAX_G);

    html += R"rawliteral(">
</div>

<hr>
<div class="sliderRow">

<label>Red Brightness:
    <span id="Red_brightVal">
)rawliteral";

    html += String(cfg.Red_brightness);

    html += R"rawliteral(
    </span>
</label>

<input
    type="range"
    min="1"
    max="255"
    step="1"
    name="Red_brightness"
    value=")rawliteral";

    html += String(cfg.Red_brightness);

    html += R"rawliteral("
    oninput="document.getElementById('Red_brightVal').innerHTML=this.value">

</div>

<div class="row">
<label>XR1</label>
<input type="number" step="1" name="xr1" value=")rawliteral";

    html += String(cfg.XR1);

    html += R"rawliteral(">

<label>YR1</label>
<input type="number" step="1" name="yr1" value=")rawliteral";

    html += String(cfg.YR1);

    html += R"rawliteral(">
</div>

<div class="row">
<label>XR2</label>
<input type="number" step="1" name="xr2" value=")rawliteral";

    html += String(cfg.XR2);

    html += R"rawliteral(">

<label>YR2</label>
<input type="number" step="1" name="yr2" value=")rawliteral";

    html += String(cfg.YR2);

    html += R"rawliteral(">
</div>

<div class="row">
<label>XR3</label>
<input type="number" step="1" name="xr3" value=")rawliteral";

    html += String(cfg.XR3);

    html += R"rawliteral(">

<label>YR3</label>
<input type="number" step="1" name="yr3" value=")rawliteral";

    html += String(cfg.YR3);

    html += R"rawliteral(">
</div>

<div class="row">
<label>RED MAX TIME:</label>
<input type="number" step="1" name="maxr" value=")rawliteral";

    html += String(cfg.MAX_R);

    html += R"rawliteral(">
</div>

<button class="save" type="submit">
SAVE
</button>

</form>

<form action="/default" method="POST">
<button class="default" type="submit">
LOAD DEFAULT
</button>
</form>

<form action="/restart" method="POST">
<button class="restart" type="submit">
RESTART
</button>
</form>

</div>

</body>
</html>

)rawliteral";

    return html;
}

//====================================================
// WEB HANDLERS
//====================================================

void handleRoot()
{
  server.send(200, "text/html", makePage());
}

void handleSave()
{
  cfg.Green_brightness = constrain(server.arg("Green_brightness").toInt(), 1, 255);
  cfg.Red_brightness = constrain(server.arg("Red_brightness").toInt(), 1, 255);
  cfg.XG1 = server.arg("xg1").toInt();
  cfg.YG1 = server.arg("yg1").toInt();

  cfg.XG2 = server.arg("xg2").toInt();
  cfg.YG2 = server.arg("yg2").toInt();

  cfg.XG3 = server.arg("xg3").toInt();
  cfg.YG3 = server.arg("yg3").toInt();

  cfg.MAX_G = server.arg("maxg").toInt();

  cfg.XR1 = server.arg("xr1").toInt();
  cfg.YR1 = server.arg("yr1").toInt();

  cfg.XR2 = server.arg("xr2").toInt();
  cfg.YR2 = server.arg("yr2").toInt();

  cfg.XR3 = server.arg("xr3").toInt();
  cfg.YR3 = server.arg("yr3").toInt();

  cfg.MAX_R = server.arg("maxr").toInt();

  saveConfig();

  server.send(200,
              "text/html",
              "<h2>Saved Successfully</h2>"
              "<a href='/'>Back</a>");

  delay(2000);

  ESP.restart();
}

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
  int32_t thoi_gian_con_lai = Thoi_Gian_Sang_Xanh - hientai;
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
  dma_display->setCursor(cfg.XG1, cfg.YG1);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(0, 255, 0));
  dma_display->println('D');
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(cfg.XG2, cfg.YG2);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(0, 255, 0));
  dma_display->println('E');
  dma_display->setCursor(cfg.XG3, cfg.YG3);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(0, 255, 0));
  dma_display->print(thoi_gian_con_lai);
  dma_display->println(';');
}

void drawText_Red()
{
  uint32_t hientai = (millis() - RedStartTime) / 1000;
  int32_t thoi_gian_con_lai = Thoi_Gian_Sang_Do - hientai;
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

  dma_display->setCursor(cfg.XR1, cfg.YR1);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(255, 0, 0));
  dma_display->println('C');
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(cfg.XR2, cfg.YR2);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(dma_display->color565(255, 0, 0));
  dma_display->println('E');

  dma_display->setCursor(cfg.XR3, cfg.YR3);   // start at top left, with 8 pixel of spacing
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
void drawText_Setting_Mode()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(2, 7);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_SETUP_WIFI);
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(2, 15);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_SETUP_PASS);

  dma_display->setCursor(2, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println(DEFAULT_IP); 
}

void drawText_Wellcome()
{
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setFont(&kongtext4pt7b);
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(2, 7);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println("WELLCOME");
  // dma_display->setFont(&kongtext4pt7b);
  dma_display->setCursor(2, 15);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println("TO SEI");

  dma_display->setCursor(2, 23);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->println("VIETNAM");

  dma_display->setCursor(2, 31);    // start at top left, with 8 pixel of spacing
  dma_display->setTextColor(myGREEN);
  dma_display->print("V:"); 
  dma_display->println(FW_VERSION);



}


void setup() {

  // Module configuration
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  loadConfig();
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );
  pinMode(GREEN_SIG, INPUT);
  pinMode(RED_SIG, INPUT);
  pinMode(SETUP_SIG, INPUT);
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(cfg.Green_brightness); //0-255
  dma_display->clearScreen();
  myBLACK = dma_display->color565(0, 0, 0);
  myWHITE = dma_display->color565(255, 255, 255);
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);
  drawText_Wellcome();
  if ((digitalRead(SETUP_SIG) == LOW))
  {
    delay(1000);
    if ((digitalRead(SETUP_SIG) == LOW))
    {
      WiFi.begin(DEFAULT_WIFI_OTA, DEFAULT_PASS_OTA);
      dma_display->clearScreen();
      delay(100);
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
      delay(100);
      drawText_OTA_updating();
      result_ota_update = checkAndUpdateFirmware();  // gọi hàm OTA
      dma_display->clearScreen();
      drawText_OTA_Successfully();
      while(1);
    }
  }
  Thoi_Gian_Sang_Xanh = cfg.MAX_G;
  Thoi_Gian_Sang_Do = cfg.MAX_R;
  delay(3000);
}

void loop() {

  if (digitalRead(SETUP_SIG) == LOW )
      {
          delay(100);
          int count = 0;
          for(int i = 0; i < 100; i++)
          {
            if(digitalRead(SETUP_SIG) == HIGH)
            {
              count = 0;
            }
            delay(10);
            count++;
          }
          if((digitalRead(SETUP_SIG) == LOW) && (count > 90))
          {
              setup_mode = 1;
              SetupStartTime = millis();
              dma_display->clearScreen();
              drawText_Setting_Mode();
              WiFi.mode(WIFI_AP);

              WiFi.softAP(DEFAULT_SETUP_WIFI, DEFAULT_SETUP_PASS);

              Serial.println();
              Serial.println("WiFi AP Started");
              Serial.print("IP: ");
              Serial.println(WiFi.softAPIP());

              server.on("/", handleRoot);

              server.on("/save",
                        HTTP_POST,
                        handleSave);
              server.begin();
          }
      }
    if (setup_mode == 1)
    {
        server.handleClient();
        uint32_t hientai = (millis() - SetupStartTime) / 1000;
        if(hientai > 60)
        {
          dma_display->clearScreen();
          setup_mode = 0;
          ESP.restart();
        }
    }
    if((digitalRead(GREEN_SIG)==LOW) && (GreenStatus == 0) && (digitalRead(RED_SIG)==HIGH))
    {
      int count = 0;
      for(int i = 0; i < 10; i++)
      {
        if(digitalRead(GREEN_SIG) == HIGH)
        {
          count = 0;
        }
        delay(1);
        count++;
      }
      if((digitalRead(GREEN_SIG)==LOW) && (GreenStatus == 0) && (digitalRead(RED_SIG)==HIGH) && (count >= 8))
        {
          setup_mode = 0;
          GreenStartTime = millis();
          dma_display->clearScreen();
          dma_display->setBrightness8(cfg.Green_brightness);
          GreenStatus = 1;
        }
    }

    if((digitalRead(RED_SIG)==LOW) && (RedStatus == 0) && (digitalRead(GREEN_SIG)==HIGH))
    {
      int count = 0;
      for(int i = 0; i < 10; i++)
      {
        if(digitalRead(RED_SIG) == HIGH)
        {
          count = 0;
        }
        delay(1);
        count++;
      }
      if((digitalRead(RED_SIG)==LOW) && (RedStatus == 0) && (digitalRead(GREEN_SIG)==HIGH) && (count >= 8))
      {
        RedStartTime = millis();
        setup_mode = 0;
        dma_display->clearScreen();
        dma_display->setBrightness8(cfg.Red_brightness);
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
    if((GreenStatus == 0) && (RedStatus == 0) && (setup_mode == 0))
    {
      dma_display->clearScreen();
    }
}
