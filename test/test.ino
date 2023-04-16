#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define buzzer 9
#define reed_switch 7
#define alarm_toggle 6
#define red_led 4
#define green_led 3

#define button A1

void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(alarm_toggle, INPUT_PULLUP);
  pinMode(reed_switch, INPUT);
  pinMode(button, INPUT_PULLUP);
  noTone(buzzer);

  leds__test();
  buzzer__test();
}

void loop() {
  //alarm_toggle__test();
  //button__test();
  //RFID__test();
  reed__test();
}

void alarm_toggle__test() {
  Serial.println(digitalRead(alarm_toggle));
}

void leds__test() {
  digitalWrite(green_led, 1);
  Serial.println("GREEN LED - ON");
  delay(2000);
  digitalWrite(green_led, 0);
  Serial.println("GREEN LED - OFF");

  digitalWrite(red_led, 1);
  Serial.println("RED LED - ON");
  delay(2000);
  digitalWrite(red_led, 0);
  Serial.println("RED LED - OFF");
}

void button__test() {
  Serial.println(digitalRead(button));
}

void RFID__test() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
}

void reed__test() {
  Serial.println(digitalRead(reed_switch));
}

void buzzer__test() {
  for (int i = 2; i > 0; i = i - 1) {
    analogWrite(buzzer, 25);
    delay(100);
    analogWrite(buzzer, 0);
    delay(200);
  }
}