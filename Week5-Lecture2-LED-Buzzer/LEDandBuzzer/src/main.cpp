#include <Arduino.h>

// ---------- LED 1 Setup ----------
#define LED1_PIN   18
#define LED1_CH    0
#define LED_FREQ   5000
#define LED_RES    8

// ---------- LED 2 Setup ----------
#define LED2_PIN   19
#define LED2_CH    1

// ---------- Buzzer Setup ----------
#define BUZZER_PIN 27
#define BUZZER_CH  2
#define BUZZER_RES 10

// ---------- Melody ----------
int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
int noteCount = sizeof(melody) / sizeof(melody[0]);

void setup() {
  // --- LED 1 PWM setup ---
  ledcSetup(LED1_CH, LED_FREQ, LED_RES);
  ledcAttachPin(LED1_PIN, LED1_CH);

  // --- LED 2 PWM setup ---
  ledcSetup(LED2_CH, LED_FREQ, LED_RES);
  ledcAttachPin(LED2_PIN, LED2_CH);

  // --- Buzzer PWM setup ---
  ledcSetup(BUZZER_CH, 2000, BUZZER_RES);
  ledcAttachPin(BUZZER_PIN, BUZZER_CH);
}

void loop() {
  // --- Play melody while both LEDs fade ---
  for (int i = 0; i < noteCount; i++) {
    ledcWriteTone(BUZZER_CH, melody[i]); // play tone

    // LED1 fades in while LED2 fades out
    for (int d = 0; d <= 255; d++) {
      ledcWrite(LED1_CH, d);         // LED1 fade in
      ledcWrite(LED2_CH, 255 - d);   // LED2 fade out
      delay(5);
    }

    delay(150); // small pause between tones
  }

  // --- Turn everything off ---
  ledcWrite(BUZZER_CH, 0);
  ledcWrite(LED1_CH, 0);
  ledcWrite(LED2_CH, 0);
  delay(500);
}
