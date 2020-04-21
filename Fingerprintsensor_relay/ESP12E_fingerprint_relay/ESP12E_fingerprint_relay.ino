/*
  Project: ESP8266 ESP-12E module (Nodemcu), Fingerprint Sensor DY50, 3.3VDC relay madule, solenoid lock system - Access by finger print
  Function: Solenoid lock will open when you provide correct finger print
  ACOPTEX.COM
*/
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(13, 15);//Fingerprint sensor TX attached to D7 (GPIO13), RX - to D8 (GPIO15) of Nodemcu board

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;
int passDelay = 6000;//Delay to pass the door

const int relayPin = 5; //Relay pin attached to D1 (GPIO5) of Nodemcu board
const int buzzerPin = 4; //Buzzer Pin attached to D2 (GPIO4) of Nodemcu board
void setup()
{
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(relayPin, LOW);//Solenoid lock closed by default
  Serial.begin(115200);//Initialize serial communication at 115200 bps
  Serial.println("Automatic door lock system - Acoptex.com");
  finger.begin(57600);//Set the data rate for the fingerprint sensor serial port

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor ready for operation!");
    Serial.println();
    Serial.println();
  } else {
    Serial.println("Did not find fingerprint sensor. Check your wiring...");
    Serial.println();
    Serial.println();
    while (1) {
      delay(1);
    }
  }
  Serial.println("Provide your finger to activate door lock....");
}

void loop()
{
  getFingerPrintID();
   
  delay(50);
}

// returns -1 if failed, otherwise returns ID #
int getFingerPrintID() {
  
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    return -1;
  }
  

  // found a match!
  Serial.print("Found ID #"); 
  Serial.println(finger.fingerID);
   if (finger.fingerID == 1 && finger.confidence > 100) {
    Buzzer();
    digitalWrite(relayPin, HIGH);
    Serial.println("Welcome to Acoptex.com. Pass the door");
    delay(passDelay);
    digitalWrite(relayPin, LOW);
    Serial.println("Door closed");
    Serial.println("\n\nProvide your finger to activate door lock....");
    }

  return finger.fingerID;
}

void Buzzer()
{
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}
