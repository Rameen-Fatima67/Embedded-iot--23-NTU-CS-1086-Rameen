#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- OLED setup ----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA=21, SCL=22)

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;);
  }

  display.clearDisplay();
}

void loop() {
  display.clearDisplay();

  // ---- Display Name ----
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 15);
  display.println("Rameen Fatima");

  // ---- Display ID ----
  display.setCursor(30, 27);
  display.println("23-NTU-CS-1086");

  // ---- Display Section ----
  display.setCursor(40, 39);
  display.println("BSCS-5TH-B");

  // ---- Draw a larger box around the text ----
  // Wider margins for a clean look
  display.drawRect(10, 5, 108, 50, SSD1306_WHITE);
  // x = 10, y = 5 → starting position
  // width = 108, height = 50 → nice padding around all text

  display.display();
  delay(3000);
}
