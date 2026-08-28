#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

//  Display 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pins
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 11;

// Buzzer 
enum BuzzerType { BUZZER_ACTIVE, BUZZER_PASSIVE };
const BuzzerType BUZZER_TYPE = BUZZER_ACTIVE;
const int PASSIVE_TONE_FREQ_PULSE      = 2000;
const int PASSIVE_TONE_FREQ_CONTINUOUS = 3000;

//  Timing
unsigned long previousBuzzerMillis = 0;
unsigned long previousSensorMillis = 0;
unsigned long previousDisplayMillis = 0;

const long SENSOR_INTERVAL  = 60;
const long DISPLAY_INTERVAL = 100;

bool buzzerToneOn = false;
bool oledRadi = false;
float smoothedDistance = 200.0;

//  Zone 
const float BOUNDS[4] = {10.0, 20.0, 35.0, 50.0};
const float HYSTERESIS = 3.0;
int zoneIndex = 4;

enum BuzzerMode { BUZZER_OFF, BUZZER_PULSE, BUZZER_CONTINUOUS };

struct ZoneConfig {
  const char* label;
  int beepIntervalMs;
  BuzzerMode mode;
};
const ZoneConfig ZONES[5] = {
  {"!! STOP !!", 0,   BUZZER_CONTINUOUS},
  {"  USPORI  ", 80,  BUZZER_PULSE},
  {"  USPORI  ", 200, BUZZER_PULSE},
  {" USPORI/OK", 400, BUZZER_PULSE},
  {" SIGURNO  ", 0,   BUZZER_OFF}
};

//  Senzor 
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  float rawDistance = duration * 0.0343 / 2.0;

  if (rawDistance == 0 || rawDistance > 200) {
    rawDistance = 200;
  }
  return rawDistance;
}

void updateZone(float distance) {
  while (zoneIndex > 0 && distance <= BOUNDS[zoneIndex - 1]) {
    zoneIndex--;
  }
  while (zoneIndex < 4 && distance > BOUNDS[zoneIndex] + HYSTERESIS) {
    zoneIndex++;
  }
}

//              Buzzer logic 
void setBuzzer(bool on, BuzzerMode mode) {
  if (BUZZER_TYPE == BUZZER_ACTIVE) {
    digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
  } else {
    if (on) {
      int freq = (mode == BUZZER_CONTINUOUS) ? PASSIVE_TONE_FREQ_CONTINUOUS : PASSIVE_TONE_FREQ_PULSE;
      tone(BUZZER_PIN, freq);
    } else {
      noTone(BUZZER_PIN);
    }
  }
}

void updateBuzzer(unsigned long currentMillis) {
  const ZoneConfig& cfg = ZONES[zoneIndex];

  switch (cfg.mode) {
    case BUZZER_CONTINUOUS:
      setBuzzer(true, BUZZER_CONTINUOUS);
      break;

    case BUZZER_PULSE:
      if (currentMillis - previousBuzzerMillis >= (unsigned long)cfg.beepIntervalMs) {
        previousBuzzerMillis = currentMillis;
        buzzerToneOn = !buzzerToneOn;
        setBuzzer(buzzerToneOn, BUZZER_PULSE);
      }
      break;

    case BUZZER_OFF:
    default:
      buzzerToneOn = false;
      setBuzzer(false, BUZZER_OFF);
      break;
  }
}

// ---------- Display logic----------
void updateDisplay() {
  if (!oledRadi) return;

  const ZoneConfig& cfg = ZONES[zoneIndex];

  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Udaljenost: "));
  display.print(smoothedDistance, 1);
  display.println(F(" cm"));

  display.setTextSize(2);
  display.setCursor(10, 20);
  display.println(cfg.label);

  int barWidth = map(constrain((int)smoothedDistance, 5, 100), 5, 100, 128, 0);
  display.drawRect(0, 50, 128, 12, SH110X_WHITE);
  display.fillRect(0, 50, barWidth, 12, SH110X_WHITE);
  display.display();
}

// ---------- Setup / Loop ----------
void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (display.begin(0x3C, true)) {
    oledRadi = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(15, 20);
    display.println(F("PARKING SENZOR"));
    display.setCursor(30, 40);
    display.println(F("Inicijalizacija..."));
    display.display();
    delay(1000);
  } else {
    Serial.println(F("OLED nije pronadjen! Nastavljam rad bez zaslona."));
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousSensorMillis >= SENSOR_INTERVAL) {
    previousSensorMillis = currentMillis;
    float currentDistance = readDistance();
    smoothedDistance = (smoothedDistance * 0.6) + (currentDistance * 0.4);
    updateZone(smoothedDistance);
  }

  updateBuzzer(currentMillis);

  if (currentMillis - previousDisplayMillis >= DISPLAY_INTERVAL) {
    previousDisplayMillis = currentMillis;
    updateDisplay();
  }
}
