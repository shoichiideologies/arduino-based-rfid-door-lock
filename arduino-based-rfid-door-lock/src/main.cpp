#include <Arduino.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

#define SS_PIN 10
#define RST_PIN 9
#define RELAY_PIN 8
#define BUZZER_PIN 7

MFRC522 rfid(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte authorizedUID[] = {0xE2, 0x7E, 0x1B, 0x2F};

bool doorLocked = true;
void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("RFID Door Lock");
  Serial.println("RFID Door Lock");
  delay(2000);
  lcd.clear();

  lockDoor(); 
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
    Serial.println("Card is not present");
  }
  Serial.println("Card is present");


  if (!rfid.PICC_ReadCardSerial()) {
    return;
      Serial.println("Card cannot be read");
  }
  Serial.println("Card can be read");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scanning...");
  Serial.println("Scanning...");
  delay(2000);

  if (isAuthorized(rfid.uid.uidByte, rfid.uid.size)) {
    lcd.setCursor(0, 1);
    lcd.print("Access Granted!");
    Serial.println("Access Granted");
    accessGrantedBeep();
    delay(2000);
    unlockDoor(); 
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Access Denied!");
    Serial.println("Access Denied");
    accessDeniedBeep();
    delay(2000);
    lockDoor();
  }

  rfid.PICC_HaltA();
}

bool isAuthorized(byte *uid, byte size) {
  if (size != sizeof(authorizedUID)) {
    return false;
  }

  for (byte i = 0; i < size; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;
    }
  }

  return true;
}

void lockDoor() {
  digitalWrite(RELAY_PIN, LOW); 
  doorLocked = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Locked");
  Serial.println("Door Locked");
  delay(2000);
}

void accessGrantedBeep(){
  tone(BUZZER_PIN, 1000, 200);
  delay(200);
  tone(BUZZER_PIN, 1500,200);
}

void accessDeniedBeep(){
  tone(BUZZER_PIN, 250, 250);
}

void unlockDoor() {
  digitalWrite(RELAY_PIN, HIGH); 
  doorLocked = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Door Unlocked");
  Serial.println("Door Unlocked");
  delay(10000); // Keep door unlocked for 10 seconds

  lockDoor();  // Auto-lock after 10 seconds
}
