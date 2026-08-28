#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 11;

unsigned long previousBuzzerMillis = 0;
unsigned long previousSensorMillis = 0;
const long SENSOR_INTERVAL = 60;

bool buzzerState = false;
bool oledRadi = false;
float smoothedDistance = 200.0;

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

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    oledRadi = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
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
  }

  int beepInterval = 0; 
  if (smoothedDistance <= 10) {
    beepInterval = -1;
  } else if (smoothedDistance <= 20) {
    beepInterval = 80;
  } else if (smoothedDistance <= 35) {
    beepInterval = 200;
  } else if (smoothedDistance <= 50) {
    beepInterval = 400;
  } else {
    beepInterval = 0;
  }

  if (beepInterval == -1) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else if (beepInterval > 0) {
    if (currentMillis - previousBuzzerMillis >= (unsigned long)beepInterval) {
      previousBuzzerMillis = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    }
  } else {
    buzzerState = false;
    digitalWrite(BUZZER_PIN, LOW);
  }

  if (oledRadi) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Udaljenost: "));
    display.print(smoothedDistance, 1);
    display.println(F(" cm"));

    display.setTextSize(2);
    display.setCursor(10, 20);

    if (smoothedDistance <= 10) {
      display.println(F("!! STOP !!"));
    } else if (smoothedDistance <= 35) {
      display.println(F("  USPORI  "));
    } else if (smoothedDistance <= 50) {
      display.println(F(" USPORI/OK"));
    } else {
      display.println(F(" SIGURNO  "));
    }

    int barWidth = map(constrain((int)smoothedDistance, 5, 100), 5, 100, 128, 0);
    display.drawRect(0, 50, 128, 12, SSD1306_WHITE);
    display.fillRect(0, 50, barWidth, 12, SSD1306_WHITE);

    display.display();
  }
}
