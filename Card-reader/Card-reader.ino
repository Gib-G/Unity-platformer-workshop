#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

// MFRC522 setup.

#define RC522_RST D15
#define RC522_SDA D14 // SDA/SS pin.

MFRC522 mfrc522(RC522_SDA, RC522_RST);

// LCD setup.

#define LCD_RS D2
#define LCD_ENABLE D3
#define LCD_D4 D4
#define LCD_D5 D5
#define LCD_D6 D6
#define LCD_D7 D7

// Should be named "lcd" : this global variable is used inside
// a bunch of functions.
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// LEDs setup.

#define LED_RED D9
#define LED_GREEN D8

// FORWARD DECLARATIONS OF USEFUL FUNCTIONS

// Makes red and/or green LEDs blink numberOfBlinks times
// periodically (cf. "period" parameter in ms).
// red == true for the red LED to blink.
// green == true for the green LED to blink.
// If red && green == true, the LEDs blink alternatively.
void ledBlink(bool red, bool green, int numberOfBlinks, unsigned int period);

// Prints message to LCD.
// If the message is too long to fit in
// one line, it is split between line 1 and 2 of the LCD.
// Each line of the LCD is 16-characters long.
// This split process makes sure not to chop words if possible.
// The later is impossible only if the first word of the message is too large to
// fit in one line. In that case, such a word is split between line 1
// and line 2 with a hyphen.
// If the message is too long to fit 2 lines (of 16 characters each),
// the end of it is discarded and not dispalyed.
void lcdPrint(String const& message);

// Clears the LCD.
void lcdClear();

// Called upon initialization (when "init" is received on
// the serial bus.
void initialize();

// Prints message on LCD numberOfPrints times.
// For each print, " /", " -", " \", or " |" is appended
// to message to produce a loading effect.
// Each print is separated by timeStep ms.
void process(int numberOfPrints, unsigned int timeStep, String message);

// Waits for the user to scan their card on the reader.
// Returns false if no card is presented and blocking is
// set to false. Returns true if a card is presented.
// If blocking is set to true, this function doesn't
// return until a card is presented to the reader.
bool waitForCard(bool blocking);

// A flag that is set to true when the Unity part
// initiates the init phase ("init" is received via the
// serial bus).
bool initializing{false};

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  // Initialisation du port série.
  Serial.begin(115200);
  while (!Serial);

  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
}

void loop() {
  if (!waitForCard(false)) {
    return;
  }
  // User scans their card...
  // ...during the init phase.
  if (initializing) {
    process(20, 150, "Scanning");
    lcdPrint("Access granted!");
    ledBlink(false, true, 30, 100);
    delay(1000);
    lcdClear();
    initializing = false;
    Serial.println("init:ok");
  }
  // ...after the init phase (in-game).
  else {
    Serial.println("card");
  }
}

void serialEvent() {
  while (Serial.available()) {
    // Gets the last message sent by the Unity part.
    String message{Serial.readStringUntil('\r')};
    // Gets rid of the remaining garbage in the serial bus.
    Serial.read();
    // Actions based on the type of message received.
    // Initialization phase.
    if (message == "init") {
      lcdClear();
      ledBlink(true, true, 20, 80);
      lcdPrint("Please scan your badge! ->");
      initializing = true;
    }
    // We receive the number of a terminal.
    else if (message == "1"
             || message == "2"
             || message == "3"
             || message == "4"
            ) {
      lcdPrint("Terminal " + message);
      ledBlink(true, true, 2, 350);
    }
    // The player just move away from a terminal.
    else if (message == "nan") {
      lcdClear();
    }
    // Terminal activated!
    else if (message == "card:ok") {
      lcdPrint("Terminal activated!");
      ledBlink(false, true, 15, 100);
    }
    // Auth to terminal failed (wrong ID).
    else if (message == "card:ko") {
      lcdPrint("Access denied!");
      ledBlink(true, false, 15, 100);
    }
    // The player has hacked a terminal.
    else if (message == "hacked") {
      lcdPrint("#&??+--#!(*###o^?_|/!#,??-++/##");
      ledBlink(true, true, 2, 400);
    }
    // Simulates the player writing a new employee ID
    // into their card/badge.
    else if (message == "write") {
      lcdPrint("Scan your badge to rewrite ID!");
      ledBlink(true, true, 1, 100);
      // Waiting for the user to scan their badge/card.
      waitForCard(true);
      process(20, 150, "Writing new     ID");
      lcdPrint("Done! Badge reconfigured.");
      ledBlink(false, true, 15, 100);
      Serial.println("write:ok");
    }
    // The Unity part sends the ID stored on the card.
    else if (message.substring(0, 3) == "id:") {
      lcdPrint("Employee ID is  " + message.substring(3));
      ledBlink(false, true, 2, 300);
    }
    // Manual control.
    else if (message == "led_green_on") digitalWrite(LED_GREEN, true);
    else if (message == "led_green_off") digitalWrite(LED_GREEN, false);
    else if (message == "led_red_on") digitalWrite(LED_RED, HIGH);
    else if (message == "led_red_off") digitalWrite(LED_RED, LOW);
  }
}

void ledBlink(bool red, bool green, int numberOfBlinks, unsigned int period) {
  bool toggle{true};
  for (int k = 0; k < 2 * numberOfBlinks; k++) {
    digitalWrite(LED_RED, red && toggle);
    digitalWrite(LED_GREEN, green && !toggle);
    toggle = !toggle;
    delay(period / 2);
  }
  digitalWrite(LED_RED, false);
  digitalWrite(LED_GREEN, false);
}

void lcdPrint(String const& message) {
  int lines = min(2, (int)(message.length() / 16 + 1));
  lcd.begin(16, lines);
  lcd.clear();
  if (lines > 1) {
    String line1{};
    String line2{};
    String temp = message.substring(0, 17);
    int index = temp.lastIndexOf(" ");
    // A word is split between line 1 and 2 (index < 15)
    // or the last character of line 1 is a whitespace (index == 15).
    if (0 < index && index <= 15) {
      line1 = message.substring(0, index + 1);
      line2 = message.substring(index + 1);
    }
    // Line 2 starts with a whitespace.
    // We can remove it.
    else if (index > 15) {
      line1 = message.substring(0, 16);
      line2 = message.substring(17);
    }
    // The first word in the message is too
    // large to fit one line.
    // We split it in half with a hyphen.
    else {
      line1 = message.substring(0, 16);
      line1.setCharAt(15, '-');
      line2 = message.substring(15);
    }
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    return;
  }
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void lcdClear() {
  lcd.begin(16, 2);
  lcd.clear();
}

void initialize() {
  lcdClear();
  ledBlink(true, true, 20, 80);
  lcdPrint("Please scan your badge! ->");
  waitForCard(true);
  process(20, 150, "Scanning");
  lcdPrint("Access granted!");
  ledBlink(false, true, 30, 100);
  delay(1000);
  lcdClear();
  Serial.println("init:ok");
}

void process(int numberOfPrints, unsigned int timeStep, String message) {
  String temp{message};
  for (int k = 0; k < numberOfPrints; k++) {
    if (k % 4 == 3) {
      temp = message;
    }
    temp += ".";
    lcdPrint(temp);
    delay(timeStep);
  }
}

bool waitForCard(bool blocking) {
  // Resetting the RFID card reader.
  SPI.begin();
  mfrc522.PCD_Init();
  if (blocking) {
    while (!(mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())) {}
    return true;
  }
  else {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      return true;
    }
    return false;
  }
}
