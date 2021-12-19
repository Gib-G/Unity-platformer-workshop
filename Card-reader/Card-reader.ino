#include <SPI.h>
#include <MFRC522.h>

// Affectation des broches
#define RST_PIN 5
#define SDA_PIN 4

MFRC522 mfrc522(SDA_PIN, RST_PIN);

void setup() {
  
 // Initialisation du Module RFID
 Serial.begin(115200);
 
 while (!Serial);
 SPI.begin();
 mfrc522.PCD_Init();
 mfrc522.PCD_DumpVersionToSerial(); // Affichage des données de la bibliothèque
 Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
 
}

void loop() {
  
 // Attente d'une carte RFID
 if ( ! mfrc522.PICC_IsNewCardPresent()) {
 return;
 }
 
 // Récupération des informations de la carte RFID
 if ( ! mfrc522.PICC_ReadCardSerial()) {
 return;
 }
 
 // Affichage des informations de la carte RFID
 mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

 // Added.
 // Modification de l'UID de la carte.
 /*
 byte newUid[] = {0x00, 0x00, 0x00, 0x01};
 if ( mfrc522.MIFARE_SetUid(newUid, (byte)4, true) ) {
   Serial.println(F("Wrote new UID to card."));
 }
 */
 
}
