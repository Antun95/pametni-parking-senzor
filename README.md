Pametni Parking Senzor

Arduino Nano projekt koji mjeri udaljenost do prepreke pomoću ultrazvučnog senzora, prikazuje rezultat na OLED ekranu i upozorava zvukom kad se previše približimo.

Kako radi
- HC-SR04 senzor kontinuirano mjeri udaljenost do prepreke ispred sebe.
- OLED ekran prikazuje trenutnu udaljenost u cm, tekstualnu poruku i progress bar koji se puni što smo bliže prepreci.
- Buzzer pišti sve brže što je prepreka bliža, a na manje od 10 cm daje neprekidan ton.
- Sustav koristi `millis()` umjesto `delay()`, pa senzor, ekran i buzzer rade neovisno jedno o drugom bez zamrzavanja.

Komponente
Arduino Nano:  upravlja cijelim sustavom 
HC-SR04: mjeri udaljenost ultrazvukom 
OLED ekran (SH1106, 128x64, I2C) : prikaz udaljenosti i poruka 
Aktivni buzzer: zvučno upozorenje 

Shema spajanja

| Komponenta | Pin | Arduino Nano |
| HC-SR04 | VCC | 5V |
| HC-SR04 | GND | GND |
| HC-SR04 | Trig | D9 |
| HC-SR04 | Echo | D10 |

| OLED | VCC | 5V |
| OLED | GND | GND |
| OLED | SDA | A4 |
| OLED | SCL | A5 |

| Buzzer | + | D11 |
| Buzzer | - | GND |

Zone udaljenosti

| Udaljenost | Poruka | Buzzer |
| > 50 cm | SIGURNO | isključen |
| 35-50 cm | SIGURNO/USPORI | sporo pištanje |
| 20-35 cm | USPORI | srednje pištanje |
| 10-20 cm | USPORI | brzo pištanje |
| < 10 cm | STOP | neprekidan ton |

 Potrebne biblioteke

- `Adafruit GFX Library`
- `Adafruit SH110X`


