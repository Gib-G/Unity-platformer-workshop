#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

// MFRC522 pins.
#define RC522_RST D15
#define RC522_SDA D14 // SDA/SS pin.

// LCD pins.
#define LCD_RS D2
#define LCD_ENABLE D3
#define LCD_D4 D4
#define LCD_D5 D5
#define LCD_D6 D6
#define LCD_D7 D7

// LEDs
#define LED_RED D9
#define LED_GREEN D8

MFRC522 mfrc522(RC522_SDA, RC522_RST);

byte bufferIn[16] = "Hello";
byte bufferOut[18];

//int block = 2;

MFRC522::MIFARE_Key key;

MFRC522::StatusCode status;

void setup() {

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

  lcd.begin(16,1);
  lcd.print("Hello world!");

  // Initialisation du port série.
  Serial.begin(115200);
  while (!Serial);

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

  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);

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
