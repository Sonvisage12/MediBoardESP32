#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>  // Include WiFiManager
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>

// ===== Display config =====
#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 4   // 4 panels chained → 256x32 display
int x =0;
MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK, myBLUE, myRED;

int scrollX = 0;   // position for scrolling text
String scrollMessage = "";

// ===== API config =====
const char* host = "api.mediboards.io";
const String path = "/api/public/hospitals/687f47fe-2429-4465-8b59-18432a3195fe/latest-patient";
unsigned long lastFetch = 0;
const unsigned long fetchInterval = 30000; // 30 seconds

// ===== Fetch data from API =====
void fetchAndUpdateMessage() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = String("https://") + host + path;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    payload.trim(); // remove spaces/newlines
    Serial.println("API Response:");
    Serial.println(payload);

    if (payload.length() > 0) {
      scrollMessage = "NEW PATIENT AT COUCH " + payload + "   ";
    } else {
      scrollMessage = "WELCOME TO NOH A&E DEPT.   OUR MOTTO: SERVICE, INTEGRITY, EMPATHY AND INNOVATION   ";
    }
  } else {
    Serial.printf("HTTP Error: %d\n", httpCode);
  }

  http.end();
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);

  // ===== Initialize WiFi using WiFiManager =====
  WiFiManager wm;
  wm.setTimeout(180);  // 3 minutes
  bool res = wm.autoConnect("NOH displayBoard");

  if (!res) {
    Serial.println("❌ Failed to connect. Restarting...");
    ESP.restart();
    delay(1000);
  }

  Serial.println("✅ Connected to WiFi!");

  // ===== Initialize LED Matrix Display =====
  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

  mxconfig.gpio.r1 = 25;
  mxconfig.gpio.g1 = 26;
  mxconfig.gpio.b1 = 27;
  mxconfig.gpio.r2 = 14;
  mxconfig.gpio.g2 = 12;
  mxconfig.gpio.b2 = 13;
  mxconfig.gpio.a  = 23;
  mxconfig.gpio.b  = 22;
  mxconfig.gpio.c  = 5;
  mxconfig.gpio.d  = 17;
  mxconfig.gpio.e  = 32;
  mxconfig.gpio.lat= 4;
  mxconfig.gpio.oe = 15;
  mxconfig.gpio.clk= 16;

  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  if (!dma_display->begin()) {
    Serial.println("❌ Matrix init failed!");
    while (1);
  }

  dma_display->setBrightness8(90);
  myBLACK = dma_display->color565(0, 0, 0);
  myRED   = dma_display->color565(255, 0, 0);
   myBLUE = dma_display->color565(100, 100, 255);
  scrollX = dma_display->width();

  fetchAndUpdateMessage();
  lastFetch = millis();
}

// ===== Loop =====
void loop() {
 if (x==0) {
   dma_display->setTextSize(2);
  dma_display->setTextColor(myBLUE);
  dma_display->setCursor(40, 1);
  dma_display->print("SONVISAGE");
   dma_display->setCursor(40, 17);
  dma_display->print("MEDIBOARD");
  delay(10000);
  x=1;
  }

  // Clear screen
  dma_display->fillScreen(myBLACK);

  // Static header
  dma_display->setTextSize(2);
  dma_display->setTextColor(myRED);
  dma_display->setCursor(10, 1);
  dma_display->print("N.O.H A&E DEPARTMENT");

  // Scrolling text
  dma_display->setTextSize(2);
  dma_display->setTextColor(myRED);
  dma_display->setCursor(scrollX, 17);
  dma_display->print(scrollMessage);

  scrollX--;
  int16_t textWidth = scrollMessage.length() * 12;
  if (scrollX < -textWidth) {
    scrollX = dma_display->width();
    fetchAndUpdateMessage();
  }

  delay(50);
}
