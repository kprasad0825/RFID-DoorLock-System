#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

String UID = "13 7C C9 34";  // Authorized RFID tag UID
byte lockState = 0;

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  servo.attach(3);
  servo.write(70);             // Locked position
  lcd.init();
  lcd.backlight();
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID Door Lock System Ready.");
}

void loop() {
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.setCursor(1, 1);
  lcd.print("Put your card");

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  lcd.clear();
  lcd.print("Scanning");
  Serial.print("NUID tag is:");

  String ID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    lcd.print(".");
    ID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    ID.concat(String(rfid.uid.uidByte[i], HEX));
    delay(300);
  }
  ID.toUpperCase();

  if (ID.substring(1) == UID && lockState == 0) {
    servo.write(160);
    lcd.clear();
    lcd.print("Door is open");
    delay(1500);
    lockState = 1;
  } else if (ID.substring(1) == UID && lockState == 1) {
    servo.write(70);
    lcd.clear();
    lcd.print("Door is locked");
    delay(1500);
    lockState = 0;
  } else {
    lcd.clear();
    lcd.print("Wrong card!");
    delay(1500);
  }
  lcd.clear();
  rfid.PICC_HaltA();
}