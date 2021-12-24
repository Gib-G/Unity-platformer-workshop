#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

// MFRC522 setup.

#define RC522_RST D15
#define RC522_SDA D14 // SDA/SS pin.

MFRC522 mfrc522(RC522_SDA, RC522_RST);

byte bufferIn[16] = "Hello";
byte bufferOut[18];

//int block = 2;

MFRC522::MIFARE_Key key;

MFRC522::StatusCode status;

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
// periodically (cf. "period" parameter).
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
void lcdPrint(String const message);

// Clears the LCD.
void lcdClear();

void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  // Initialisation du port série.
  Serial.begin(115200);
  while (!Serial);

  ledBlink(true, true, 10, 100);
  lcdPrint("My name is Giovanni Giorgio!");
  delay(5000);
  lcdPrint("All your base are belong to us!");
  delay(5000);
  lcdPrint("azeazazeazeazeazeazezedgd");
  delay(5000);
  lcdPrint("Hello!");
  delay(1000);
  lcdClear();

  // Initialisation du Module RFID.
  SPI.begin();
  mfrc522.PCD_Init();

  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  // No need.
  // Affichage des données de la bibliothèque.
  mfrc522.PCD_DumpVersionToSerial();

  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));

}

void loop() {
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Une carte a été présentée !

  // On affiche le numéro de série de la carte.
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Affichage de l'état de la mémoire de la carte.
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  for (int block = 0; block < 63; block++) {
    if((block + 1) % 4 == 0) {continue;}
    String message = String("--- Attempting to authenticate to data block ") + String(block) + String(" with key A...");
    Serial.println(message);
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Authentication failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      //return;
    }
    else
    {
      Serial.println("SUCCESS!!!");
    }
    message = String("--- Attempting to authenticate to data block ") + String(block) + String(" with key B...");
    Serial.println(message);
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Authentication failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      //return;
    }
    else
    {
      Serial.println("SUCCESS!!!");
    }
  }

  /* Write data to the block */
  /*
  status = mfrc522.MIFARE_Write(block, bufferIn, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }

  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  */

}

void serialEvent() {
  String message = Serial.readStringUntil('\r');
  if(message == "INIT") digitalWrite(LED_GREEN, HIGH);

  // Manual control.
  else if(message == "LED_GREEN_ON") digitalWrite(LED_GREEN, true);
  else if(message == "LED_GREEN_OFF") digitalWrite(LED_GREEN, false);
  else if(message == "LED_RED_ON") digitalWrite(LED_RED, HIGH);
  else if(message == "LED_RED_OFF") digitalWrite(LED_RED, LOW);
}

void ledBlink(bool red, bool green, int numberOfBlinks, unsigned int period) {
  bool toggle{true};
  for(int k = 0; k < 2 * numberOfBlinks; k++) {
    digitalWrite(LED_RED, red && toggle);
    digitalWrite(LED_GREEN, green && !toggle);
    toggle = !toggle;
    delay(period / 2);
  }
  digitalWrite(LED_RED, false);
  digitalWrite(LED_GREEN, false);
}

void lcdPrint(String const message) {
  int lines = min(2, (int)(message.length() / 16 + 1));
  lcd.begin(16, lines);
  lcd.clear();
  if(lines > 1) {
    String line1{};
    String line2{};
    String temp = message.substring(0, 17);
    int index = temp.lastIndexOf(" ");
    // A word is split between line 1 and 2 (index < 15)
    // or the last character of line 1 is a whitespace (index == 15).
    if(0 < index && index <= 15) {
      line1 = message.substring(0, index + 1);
      line2 = message.substring(index + 1);
    }
    // Line 2 starts with a whitespace.
    // We can remove it.
    else if(index > 15) {
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
  lcd.clear();
}
