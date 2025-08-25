#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>

#define PANEL_RES_X 64
#define PANEL_RES_Y 32
#define PANEL_CHAIN 4

MatrixPanel_I2S_DMA *dma_display = nullptr;
uint16_t myBLACK, myRED, myGREEN, myBLUE;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting matrix test...");

  HUB75_I2S_CFG mxconfig(PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN);

  // Double-check these GPIO assignments match your wiring!
  mxconfig.gpio.r1 = 25;
  mxconfig.gpio.g1 = 26;
  mxconfig.gpio.b1 = 27;
  mxconfig.gpio.r2 = 14;
  mxconfig.gpio.g2 = 12;
  mxconfig.gpio.b2 = 13;
  mxconfig.gpio.a = 23;
  mxconfig.gpio.b = 22;
  mxconfig.gpio.c = 5;
  mxconfig.gpio.d = 17;
  mxconfig.gpio.e = 32;     // Only needed for 64x64 or larger
  mxconfig.gpio.lat = 4;
  mxconfig.gpio.oe = 15;
  mxconfig.gpio.clk = 16;

  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  if (dma_display->begin()) {
    Serial.println("Matrix initialized successfully!");
  } else {
    Serial.println("Matrix initialization failed!");
    while(1); // halt
  }

  dma_display->setBrightness8(50);  // adjust brightness (0-255)
  dma_display->clearScreen();

  // Initialize colors
  myBLACK = dma_display->color565(0, 0, 0);
  myRED   = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE  = dma_display->color565(0, 0, 255);

  // Display text
  dma_display->setTextSize(2);                  // Text scale
  dma_display->setTextColor(myGREEN);           // Text color
  dma_display->setCursor(1, 1);                // Position (x,y)
  dma_display->print("Thank you lord Jesus");  
    dma_display->setTextSize(2);                  // Text scale
  dma_display->setTextColor(myGREEN);           // Text color
  dma_display->setCursor(3, 17);                // Position (x,y)
  dma_display->print("It is well with me");               // Print text
}

void loop() {
  // Nothing for now (static text)
}
