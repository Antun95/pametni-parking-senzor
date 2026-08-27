#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definiranje dimenzija OLED zaslona
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // Dijeli reset s Arduinom
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Definiranje pinova
const int TRIG_PIN = 9;
const int ECHO_PIN = 10;
const int BUZZER_PIN = 11;

// Varijable za mjerenje vremena i zvučni signal
unsigned long previousMillis = 0;
bool buzzerState = false;

void setup() {
  Serial.begin(9600);

  // Postavljanje pinova senzora i zujala
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicijalizacija I2C OLED zaslona (adresa 0x3C je standardna za većinu 0.96" modula)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED inicijalizacija neuspjesna!"));
    for (;;); // Zaustavi izvođenje ako zaslon nije pronađen
  }

  // Početni ekran
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println(F("PARKING SENZOR"));
  display.setCursor(30, 40);
  display.println(F("Inicijalizacija..."));
  display.display();
  delay(1500);
}

void loop() {
  // 1. Slanje ultrazvučnog impulsa (10 us)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 2. Mjerenje trajanja odziva (u mikrosekundama)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Timeout 30ms

  // 3. Preračunavanje u centimetre (brzina zvuka ~ 0.0343 cm/us)
  float distance = duration * 0.0343 / 2.0;

  // Filtriranje nevažećih mjerenja
  if (distance == 0 || distance > 200) {
    distance = 200;
  }

  // 4. Upravljanje zvučnim signalom (Buzzer)
  int beepInterval = 0; // Interval zvučnog signala u ms
  
  if (distance <= 10) {
    beepInterval = -1; // Kontinuirani ton za STOP zone (< 10 cm)
  } else if (distance <= 20) {
    beepInterval = 100;
  } else if (distance <= 35) {
    beepInterval = 250;
  } else if (distance <= 50) {
    beepInterval = 450;
  } else {
    beepInterval = 0;  // Isključeno ako je prepreka daleko (> 50 cm)
  }

  // Generiranje zvuka bez blokiranja programa (koristeći millis)
  unsigned long currentMillis = millis();
  if (beepInterval == -1) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else if (beepInterval > 0) {
    if (currentMillis - previousMillis >= (unsigned long)beepInterval) {
      previousMillis = currentMillis;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // 5. Prikaz podataka na OLED zaslonu
  display.clearDisplay();

  // Naslov
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Udaljenost: "));
  display.print(distance, 1);
  display.println(F(" cm"));

  // Određivanje statusne poruke
  display.setTextSize(2);
  display.setCursor(10, 20);

  if (distance <= 10) {
    display.println(F("!! STOP !!"));
} else if (distance <= 35) {
    display.println(F("  USPORI  "));
} else if (distance <= 50) {
    display.println(F(" USPORI/OK"));
} else {
    display.println(F(" SIGURNO  "));
}

  // Vizualna linija napretka (Progress Bar) na dnu ekrana
  int barWidth = map(constrain((int)distance, 5, 100), 5, 100, 128, 0);
  display.drawRect(0, 50, 128, 12, SSD1306_WHITE);
  display.fillRect(0, 50, barWidth, 12, SSD1306_WHITE);

  display.display();

  delay(50); // Kratka pauza za stabilnost osvježavanja
}
