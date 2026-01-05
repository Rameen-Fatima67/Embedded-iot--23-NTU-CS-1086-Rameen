
/******************
 * ESP32 Car Tracking and Anti-theft System
 * Features:
 *   - Motor control (Forward/Backward) via Blynk
 *   - Engine Lock / Unlock via Blynk
 *   - GPS tracking (ThingSpeak)
 *   - Vibration sensor + buzzer (car lift alert)
 *   - Geo-fence alert
 *   - OLED display shows all statuses
 ******************/

#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6_5Vxatku"
#define BLYNK_TEMPLATE_NAME "Dc motor"
#define BLYNK_AUTH_TOKEN "nhuPjCLAyTTX-aXTevuRp1AzhXycV2hg"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// ---------------- WiFi ----------------
char ssid[] = "NTU-FSD";
char pass[] = "";

// ---------------- ThingSpeak ----------------
String tsWriteAPIKey = "3BHWKOMC9AKKYXGA"; // Your ThingSpeak Write API Key

// ---------------- Motor Pins ----------------
#define IN1 4
#define IN2 18
#define RELAY_PIN 33
boolean relayUnlocked = true;

bool vibAlertSent = false;
bool geoAlertSent = false;
bool geoBuzzerActive = false; 

// ---------------- Vibration & Buzzer ----------------
#define VIBRATION_PIN 27
#define BUZZER_PIN 26
bool systemArmed = false; // Controlled via Serial 'A'/'D'

// ---------------- GPS ----------------
HardwareSerial GPSSerial(2); // RX=16, TX=17
TinyGPSPlus gps;

// ---------------- Geo-Fence ----------------
double centerLat = 31.462637;
double centerLng = 73.149069;
double radiusMeters = 100;

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- Last Known GPS ----------------
double lastLat = 0.0;
double lastLng = 0.0;
bool hasLastLocation = false;

// ---------------- MOTOR CONTROL ----------------
void motorStop() { digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); }
void motorForward() { if(relayUnlocked){digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);} else motorStop();}
void motorBackward(){ if(relayUnlocked){digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);} else motorStop();}

// ---------------- DISTANCE FUNCTION ----------------
double getDistance(double lat1, double lon1, double lat2, double lon2){
  const double R = 6371000;
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  lat1 = radians(lat1); lat2 = radians(lat2);
  double a = sin(dLat/2)*sin(dLat/2) + cos(lat1)*cos(lat2)*sin(dLon/2)*sin(dLon/2);
  double c = 2*atan2(sqrt(a),sqrt(1-a));
  return R*c;
}

// ---------------- BLYNK FUNCTIONS ----------------
BLYNK_WRITE(V1){ param.asInt()==1?motorForward():motorStop(); }
BLYNK_WRITE(V2){ param.asInt()==1?motorBackward():motorStop(); }
BLYNK_WRITE(V3){
  int lockState = param.asInt();
  if(lockState==1){ digitalWrite(RELAY_PIN,LOW); relayUnlocked=false; motorStop(); Serial.println("Engine LOCKED"); }
  else { digitalWrite(RELAY_PIN,HIGH); relayUnlocked=true; Serial.println("Engine UNLOCKED"); }
}
BLYNK_WRITE(V4) {  // Blynk button for ARM/DISARM
    systemArmed = param.asInt();  // 1 = ARMED, 0 = DISARMED

    if(systemArmed){
        Serial.println("System ARMED");   // Serial monitor output
    } else {
        Serial.println("System DISARMED"); 
        digitalWrite(BUZZER_PIN, LOW);    // Stop buzzer if disarmed
    }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  // Motor setup
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,HIGH); relayUnlocked=true; motorStop();

  // Vibration/Buzzer
  pinMode(VIBRATION_PIN, INPUT); pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, LOW);

  // OLED init
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){Serial.println("OLED failed"); while(true);}
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);

  // GPS init
  GPSSerial.begin(9600, SERIAL_8N1, 16, 17);

  // WiFi & Blynk
  display.setCursor(0,0); display.println("Connecting WiFi & Blynk..."); display.display();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

// ---------------- LOOP ----------------
void loop() {
  Blynk.run(); // Must be called frequently

  // Serial input for arming/disarming
  if(Serial.available()){
    char cmd = Serial.read();
    if(cmd=='A'){ systemArmed=true; Serial.println("System ARMED"); }
    if(cmd=='D'){ systemArmed=false; digitalWrite(BUZZER_PIN,LOW); Serial.println("System DISARMED"); }
  }

  // Read GPS
  while(GPSSerial.available()>0) gps.encode(GPSSerial.read());

  // Current GPS
 double currLat = lastLat;
double currLng = lastLng;
String gpsMode = "NO GPS YET";

if (gps.location.isValid()) {
  currLat = gps.location.lat();
  currLng = gps.location.lng();

  lastLat = currLat;
  lastLng = currLng;
  hasLastLocation = true;

  gpsMode = "GPS LIVE";
}
else if (hasLastLocation) {
  gpsMode = "INDOOR (LAST)";
}

  // Vibration
 if (systemArmed && digitalRead(VIBRATION_PIN) == HIGH && !vibAlertSent) {
  Serial.println("⚠️ VIBRATION DETECTED");
  digitalWrite(BUZZER_PIN, HIGH);

  Blynk.logEvent(
    "theft_alert_",
    "⚠️ Vehicle vibration detected!"
  );

  vibAlertSent = true;
}

if (digitalRead(VIBRATION_PIN) == LOW) {
  vibAlertSent = false;
  digitalWrite(BUZZER_PIN, LOW);
}

  // Geo-fence
  bool geoBreach = false; 

if(systemArmed && gps.location.isValid()){
    double dist = getDistance(currLat, currLng, centerLat, centerLng);

    // Geo-fence breach detected
    if(dist > radiusMeters && !geoAlertSent){
        geoBreach = true;
        digitalWrite(BUZZER_PIN, HIGH);
        geoAlertSent = true;
        geoBuzzerActive = true;

        Serial.println("🚨 GEO-FENCE BREACH");
        Blynk.logEvent("geofence_alert", "🚨 Vehicle moved outside geo-fence!");
    }

    // Back inside geo-fence
    if(dist <= radiusMeters && geoBuzzerActive){
        geoBreach = false;
        digitalWrite(BUZZER_PIN, LOW);
        geoBuzzerActive = false;
        geoAlertSent = false;
    }
}
// OLED display
display.clearDisplay();
display.setCursor(0,0);
display.println("ESP32 Car System");
display.print("Motor: "); if(digitalRead(IN1)) display.println("Forward"); else if(digitalRead(IN2)) display.println("Backward"); else display.println("Stopped");
display.print("Engine: "); display.println(relayUnlocked?"UNLOCKED":"LOCKED");
display.print("GPS: "); display.println(gpsMode);
display.print("Lat: "); display.println(currLat,6);
display.print("Lng: "); display.println(currLng,6);
display.print("Vibration: "); display.println((systemArmed && digitalRead(VIBRATION_PIN))?"YES":"NO");
display.print("GeoFence: "); display.println(geoBreach?"BREACH":"OK");
display.display();

  // ThingSpeak update every 5 seconds
  static unsigned long lastTS=0;
  if(millis()-lastTS>5000){
    lastTS=millis();
    if(WiFi.status()==WL_CONNECTED && tsWriteAPIKey!=""){
      HTTPClient http;
      String url="http://api.thingspeak.com/update?api_key="+tsWriteAPIKey+"&field1="+String(currLat,6)+"&field2="+String(currLng,6);
      http.begin(url);
      int code=http.GET();
      Serial.print("ThingSpeak HTTP Code: "); Serial.println(code);
      http.end();
    }
  }

  delay(50); // small delay to avoid CPU overload
}