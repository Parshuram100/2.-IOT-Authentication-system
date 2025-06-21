#include <Adafruit_Fingerprint.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

int rfA = 9;
int rfB = 10;
int rfC = 11;
int rfD = 12;

LiquidCrystal_I2C lcd(0x27, 16, 2);

Servo s1;
SoftwareSerial mySerial(2, 3); // RX, TX for fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

void setup() { 
  lcd.begin();
  lcd.backlight();

  Serial.begin(9600);
  while (!Serial);
  delay(100);

  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  s1.attach(8);
  s1.write(120); // Initial position (closed)

  pinMode(rfA, INPUT);
  pinMode(rfB, INPUT);
  pinMode(rfC, INPUT);
  pinMode(rfD, INPUT);
}

void loop() {
  int A = digitalRead(rfA);
  int B = digitalRead(rfB);
  int C = digitalRead(rfC);
  int D = digitalRead(rfD);

  if (A == 1 || D == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose FP ID:");
    delay(1000); // small delay to show message
    return;
  }

  int fingerprintID = getFingerPrint();
  if (fingerprintID >= 0) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Door Is Opened");
    lcd.setCursor(1, 1);
    lcd.print("Successfully!");
    s1.write(90); // Open
    delay(3000);
    s1.write(120); // Close to center
    delay(4000);
    lcd.clear();
    lcd.print("Door Is Closing!");
    s1.write(150); // Final close
    delay(3000);
    s1.write(120); // Reset to center
    lcd.clear();
    lcd.print("Door Is Closed !");
    delay(1000);
  } 
  else if (fingerprintID == -2) {
    lcd.clear();
    lcd.print("Fingerprint is");
    lcd.setCursor(0, 1);
    lcd.print("not registered!");
    delay(2000);
  }

  delay(50);
}

int getFingerPrint() {
  lcd.clear();
  lcd.print("Sir/Mam, Please");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Finger");

  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -2;

  return finger.fingerID;
}

uint8_t readnumber(void) 
{
  uint8_t num = 0;
  while (num == 0) 
  {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
