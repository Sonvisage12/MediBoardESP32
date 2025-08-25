#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>

#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 4   // 4 panels chained → 256x32 display

MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK, myGREEN, myRED;

int scrollX;   // position for scrolling text
const char* message = "It is well with me   "; // add spaces for smooth loop

void setup() {
  Serial.begin(115200);
  Serial.println("Starting matrix test...");

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

  // Pin config (adjust to your wiring)
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
  mxconfig.gpio.e  = 32;     // Only needed for 64px height
  mxconfig.gpio.lat= 4;
  mxconfig.gpio.oe = 15;
  mxconfig.gpio.clk= 16;

  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  if (!dma_display->begin()) {
    Serial.println("Matrix initialization failed!");
    while(1);
  }

  dma_display->setBrightness8(90);  
  dma_display->clearScreen();

  myBLACK = dma_display->color565(0, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myRED   = dma_display->color565(255, 0, 0);

  // start scrolling text just off-screen to the right
  scrollX = dma_display->width();
}

void loop() {
  // Always clear everything first
  dma_display->fillScreen(myBLACK);

  // === Top static message ===
  dma_display->setTextSize(2);         
  dma_display->setTextColor(myRED);  
  dma_display->setCursor(45, 0);      // top half (y=2)
  dma_display->print("Thank you Jesus");

  // === Bottom scrolling message ===
  dma_display->setTextSize(2);         
  dma_display->setTextColor(myRED);  
  dma_display->setCursor(scrollX, 17); // bottom half (y=18)
  dma_display->print(message);

  // move scrolling text left
  scrollX--;

  // reset once message has fully left the screen
  int16_t textWidth = strlen(message) * 12; // ~12px per char at size=2
  if (scrollX < -textWidth) {
    scrollX = dma_display->width();
  }

  delay(50); // adjust speed (smaller = faster)
}
