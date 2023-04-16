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

#define button 2

bool alarmed = true;
bool access = false;
bool allowed_to_pass = false;
bool verified = false;
bool without_access = false;
bool inside = false;

/*
   R001 = Somebody went inside without an authorized access.
   R002 = Somebody used an invalid card.

   green_led = Acess is granted.
   red_led = Access is denied or alarm is activated.
*/


void setup() {
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(alarm_toggle, INPUT_PULLUP);
  pinMode(reed_switch, INPUT);
  pinMode(button, INPUT);
  noTone(buzzer);
  delay(1000);
  Serial.println("System is ready.");
}

void loop() {
  alarm_system();  // Sets the alarm-staus by reading the alarm_toggle.

  if (alarmed == true) {
    door_system();  // Checks the reed_switch's status, to know if the door has been opened.
    read_card();    // Reads the RFID card for authorization.
    //read_button();  // Reads the button's status.
  }
}

void alarm_system() {
  if (digitalRead(alarm_toggle) == 0) {
    alarmed = false;
  } else {
    alarmed = true;
  }
}


void read_button() {
  if (digitalRead(button) == 1 && without_access == false) {
    allowed_to_pass = true;
    digitalWrite(green_led, 1);
    delay(1000 * 15);
    allowed_to_pass = false;
    digitalWrite(green_led, 0);
  }
}


void read_card() {
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
  if (content.substring(1) == "EC 72 C6 38" || content.substring(1) == "89 66 39 B2" || content.substring(1) == "B9 FF 6D B3") {
    if (without_access == true) {
      verified = true;
      return;
    }
    digitalWrite(green_led, 1);
    analogWrite(buzzer, 25);
    delay(600);
    analogWrite(buzzer, 0);
    digitalWrite(green_led, 0);
    delay(1000);
    access = true;
    if (inside == false)
    {
      inside = true;
    }
  } else {
    R002();
  }
}

void door_system() {
  if (alarmed == true && digitalRead(reed_switch) == 1) {
    if (access == false && allowed_to_pass == false) {
      if (verified == true) {
        digitalWrite(green_led, 1);
        delay(1000 * 10);
        digitalWrite(green_led, 0);
        verified = false;
        without_access = false;
      } else {
        without_access = true;
        R001();
      }
    } else {
      digitalWrite(green_led, 1);
      delay(1000 * 15);
      digitalWrite(green_led, 0);
      access = false;
    }
  }
}

// Somebody went inside without an authorized access.
void R001() {
  digitalWrite(red_led, 1);
  for (int i = 35; i > 0; i = i - 1) {
    read_card();
    digitalWrite(red_led, 1);
    analogWrite(buzzer, 50);
    delay(300);
    analogWrite(buzzer, 0);
    digitalWrite(red_led, 0);
    delay(300);
    if (verified == true) {
      break;
    }
  }
  digitalWrite(red_led, 0);
}

// Somebody used an invalid card.
void R002() {
  for (int i = 2; i > 0; i = i - 1) {
    digitalWrite(red_led, 1);
    analogWrite(buzzer, 25);
    delay(100);
    analogWrite(buzzer, 0);
    digitalWrite(red_led, 0);
    delay(200);
  }
}