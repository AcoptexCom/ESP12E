/*
  Project: ESP8266 ESP-12E module (Nodemcu), Fingerprint Sensor DY50, 3.3VDC relay madule, solenoid lock system - Access by finger print
  Function: Solenoid lock will open when you provide correct finger print. Email sent with door status through IFTTT
  ACOPTEX.COM
*/
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

const char* ssid = "TR-1";
const char* password = "12345";
const char* host = "maker.ifttt.com";
const char* iftttApiKey = "bHqghGuQv0nZhfaBKSVZ-v";//IFTTT API SECRET  KEY
bool state = true;
SoftwareSerial mySerial(13, 15);//Fingerprint sensor TX attached to D7 (GPIO13), RX - to D8 (GPIO15) of Nodemcu board

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const int redLEDPin =14;//red LED attached to D5 (GPIO14) of Nodemcu board
const int greenLEDPin =12;//green LED attached to D6 (GPIO12) of Nodemcu board
const int relayPin = 5; //Relay pin attached to D1 (GPIO5) of Nodemcu board
const int buzzerPin = 4; //Buzzer Pin attached to D2 (GPIO4) of Nodemcu board
uint8_t id;
int passDelay = 10000;//Delay to pass the door
String door_state = "";// variable for door state

void setup()
{
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(relayPin, LOW);//Solenoid lock closed by default
  digitalWrite(redLEDPin, HIGH);
  Serial.begin(115200);//Initialize serial communication at 115200 bps
  Serial.println("Automatic door lock system - Acoptex.com");
  finger.begin(57600);//Set the data rate for the fingerprint sensor serial port
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor ready for operation!");
    Serial.println();
  } else {
    Serial.println("Did not find fingerprint sensor. Check your wiring...");
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
    Serial.println("Welcome to Acoptex.com. Pass the door");
    digitalWrite(relayPin, HIGH);    
    door_state = "opened";
    digitalWrite(redLEDPin, LOW);
    digitalWrite(greenLEDPin, HIGH);
    doorStatus();
    
    delay(passDelay);
    
    digitalWrite(relayPin, LOW);
    door_state = "closed";
    digitalWrite(redLEDPin, HIGH);
    digitalWrite(greenLEDPin, LOW);
    doorStatus();
    Serial.println("Door closed");
    Serial.println("\n\nProvide your finger to activate door lock....");
  }

  return finger.fingerID;
}
void doorStatus()
{
  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
  }
  String url = "/trigger/door_status/with/key/";
  url += iftttApiKey;
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: 13\r\n\r\n" +
               "value1=" + door_state + "\r\n");
}

void Buzzer()
{
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}
