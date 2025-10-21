#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define LED1 25
#define LED2 26
#define BTN_MODE 14
#define BTN_RESET 27

int mode = 0;
unsigned long lastPress = 0;
int brightness = 0;
int fadeAmount = 10;

void setup() {
  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RESET, INPUT_PULLUP);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 25);
  display.println("System Ready!");
  display.display();
  delay(1000);
  display.clearDisplay();
}

void loop() {
  // ---- Button Handling ----
  if (!digitalRead(BTN_MODE) && millis() - lastPress > 200) {
    mode = (mode + 1) % 4;
    lastPress = millis();
  }

  if (!digitalRead(BTN_RESET) && millis() - lastPress > 200) {
    mode = 0;
    lastPress = millis();
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);

  switch (mode) {
    case 0: // Both OFF
      display.println("Mode 0: Both OFF");
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      break;

    case 1: // Alternate Blink
      display.println("Mode 1: Alternate Blink");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      delay(300);
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      delay(300);
      break;

    case 2: // Both ON
      display.println("Mode 2: Both ON");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      break;

    case 3: // PWM Fade
      display.println("Mode 3: PWM Fade");
      analogWrite(LED1, brightness);
      analogWrite(LED2, 255 - brightness);
      brightness += fadeAmount;

      if (brightness <= 0 || brightness >= 255) {
        fadeAmount = -fadeAmount;
      }
      delay(20);
      break;
  }

  display.display();
}
