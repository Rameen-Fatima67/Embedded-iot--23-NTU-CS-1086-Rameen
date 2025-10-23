//Name: Rameen Fatima                       Reg no. 23-NTU-CS-1086
//Section: BS-CS_5th B
//Title: DHT and LDR Hometask week 6

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// --- Pin Configuration ---
#define DHTPIN 14        // DHT22 data pin
#define DHTTYPE DHT11    // Sensor type: DHT22 (or DHT11 if needed)
#define LDR_PIN 36       // LDR connected to analog pin 36
#define SDA_PIN 21       // I2C SDA pin
#define SCL_PIN 22       // I2C SCL pin

// --- OLED Display Settings ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- DHT Sensor Setup ---
DHT dht(DHTPIN, DHTTYPE);

// --- Setup Function ---
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 IoT Sensor System Starting...");

  // Initialize I2C for OLED
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed!");
    while (true); // Stop if display not found
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Initializing sensors...");
  display.display();

  // Initialize DHT Sensor
  dht.begin();
  delay(2000); // Wait for sensors to stabilize
}

// --- Main Loop ---
void loop() {
  // Read DHT22 sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read LDR analog value
  int ldrValue = analogRead(LDR_PIN);
  float voltage = (ldrValue / 4095.0) * 3.3;

  // Check for invalid sensor readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // --- Serial Output ---
  Serial.println("=================================");
  Serial.printf("Temperature: %.2f °C\n", temperature);
  Serial.printf("Humidity: %.2f %%\n", humidity);
  Serial.printf("LDR ADC: %d  |  Voltage: %.2f V\n", ldrValue, voltage);
  Serial.println("=================================\n");

  // --- OLED Display Output ---
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("IoT Environment Data");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setCursor(0, 16);
  display.printf("Temp: %.1f C", temperature);

  display.setCursor(0, 28);
  display.printf("Hum : %.1f %%", humidity);

  display.setCursor(0, 40);
  display.printf("LDR : %d", ldrValue);

  display.setCursor(0, 52);
  display.printf("Volt: %.2f V", voltage);

  display.display();

  delay(2000); // Update every 2 seconds
}
