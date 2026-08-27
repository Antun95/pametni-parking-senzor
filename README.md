Pametni parking senzor


Sve pokreće Arduino Nano, sustav pomoću ultrazvuka vidi prepreke, ispisuje  udaljenost na ekranu i upozorava  zvukom ako se previše približimo.
Sustav cijelo vrijeme prati prostor ispred sebe  i radi sljedeće:
  Na OLED ekranu ispisuje točno koliko smo udaljeni od prepreke.
  Daje  vizualne upute: SIGURNO, USPORI ili STOP .
  Na dnu ekrana iscrtava progress bar koja se vizualno puni kako se približavamo zidu.
  buzzer daje ubrzaniji zvuk sto je zid bliže, ako dođemo na manje od 10 cm, zvuk postaje neprekidan.


KOMPONENTE:
Arduino Nano: Čita podatke i upravlja svime ostalim.   
HC-SR04: Šalje zvuk i sluša jeku kako bi izmjerio udaljenost.  
OLED ekran: ispisuju se upozorenja i mjerenja. 
Piezo buzzer: zvučni signal.


Arduino i ekran komuniciraju preko dvije žice kako bi se tekst na ekranu refreshao bez kašnjenja.
Umjesto da koristim naredbu `delay()` koja bi zamrznula cijeli sustav dok buzzer pišti, koristio sam ugrađenu štopericu (`millis()`). Tako Arduino može istovremeno raditi sva mjerenja i komunicirati s ekranom i buzzerom.


SPAJANJAA:
Ultrazvučni senzor (HC-SR04)
* VCC ide na 5V
* GND ide na GND
* Trig ide na pin D9
* Echo ide na pin D10

Ekran (OLED)
* VCC ide na 5V
* GND ide na GND
* SDA ide na pin A4
* SCL ide na pin A5

3. Buzzer
* + ide na pin D11
* - ide na GND


ZONE UDALJENOSTI:
  Više od 50 cm : `SIGURNO` , buzzer isključen
  Između 35 i 50 cm: `SIGURNO` / `USPORI` , Sporo pištanje. 
  Između 20 i 35 cm: `USPORI`, srednje pištanje. 
  Između 10 i 20 cm: `USPORI`, brzo pištanje.
  Manje od 10 cm: `STOP`, Neprekidan ton.

-
