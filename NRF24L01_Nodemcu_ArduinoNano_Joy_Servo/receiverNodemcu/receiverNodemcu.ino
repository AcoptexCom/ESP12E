//Receiver Side Code for ESP8266 ESP-12E module
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const int servoPin=0;//servo attached to D3 pin (GPIO0) of Nodemcu
Servo myServo;
int angleValue;

// Hardware configuration
// Set up nRF24L01 radio on pins D4 & D2 of Nodemcu
RF24 radio(2, 4);//CE, CSN
const byte pipe[6] = "00002";// Radio pipe address for the 2 nodes to communicate.

void setup() {
  Serial.begin(9600);//initialize serial communication at 9600 bps
 myServo.attach(servoPin);
  radio.begin();
  radio.openReadingPipe(0, pipe);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
   radio.read(&angleValue, sizeof(angleValue));
   myServo.write(angleValue);
   Serial.println(angleValue);
 } 
 }
