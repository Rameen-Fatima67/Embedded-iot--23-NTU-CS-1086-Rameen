/*
  ------------------------------------------------------------
  Title       : ESP32 Multi-LED Control with Press-Type Detection
  Author      : Rameen Fatima
  Reg. No.    : 23-NTU-CS-1086 (BSCS-5TH B)
  Description :
      This program detects short and long presses of a single button.
      - Short Press (<1.5 sec): Toggles 3 LEDs ON/OFF
      - Long Press (>1.5 sec): Plays a buzzer tone
      The event is displayed on the OLED (SSD1306) with a header.

      Components:
        • 1 Push Button (GPIO 14)
        • 3 LEDs (GPIO 25, 26, 27)
        • 1 Buzzer (GPIO 33)
        • 1 OLED Display (SDA=21, SCL=22)
  ------------------------------------------------------------
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- OLED Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Pin Configuration ---
#define LED1 25
#define LED2 27
#define LED3 33
#define BUZZER_PIN 26
#define BUTTON_PIN 14

// --- Variables ---
bool ledState = false;               // Stores ON/OFF state for all LEDs
unsigned long pressStart = 0;        // Time when button was pressed
bool isPressed = false;              // Flag to track button press
const unsigned long longPressTime = 1500;  // 1.5s threshold for long press

// --- Function to display header and message on OLED ---
void showOnOLED(const String &line1, const String &line2 = "") {
  display.clearDisplay();
  
  // Header area
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("PRESS-TYPE DETECT");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE); // underline

  // Main message
  display.setCursor(10, 25);
  display.println(line1);
  if (line2 != "") {
    display.setCursor(10, 40);
    display.println(line2);
  }

  display.display();
}

void setup() {
  Serial.begin(115200);

  // --- Pin Modes ---
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Button active LOW

  // --- Initialize OLED ---
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found!");
    while (true);
  }

  // --- Startup Screen ---
  showOnOLED("System Ready...", "Press Button");
  delay(1500);

  // Turn off LEDs and Buzzer initially
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);

  // --- Detect button press start ---
  if (buttonState == LOW && !isPressed) {
    isPressed = true;
    pressStart = millis();
  }

  // --- Detect button release ---
  if (buttonState == HIGH && isPressed) {
    unsigned long pressDuration = millis() - pressStart;
    isPressed = false;

    if (pressDuration < longPressTime) {
      // --- SHORT PRESS ACTION: Toggle LEDs ---
      ledState = !ledState;
      digitalWrite(LED1, ledState);
      digitalWrite(LED2, ledState);
      digitalWrite(LED3, ledState);

      showOnOLED("Short-Press Detected", ledState ? "LEDs: ON" : "LEDs: OFF");
      Serial.println(ledState ? "LEDs ON" : "LEDs OFF");

    } else {
      // --- LONG PRESS ACTION: Buzzer Tone ---
      showOnOLED("Long-Press Detected", "Playing Buzzer...");
      tone(BUZZER_PIN, 1000, 500);  // 1kHz for 0.5s
      Serial.println("Buzzer Tone Played");
    }

    delay(300); // Small delay to avoid flicker
  }
}
