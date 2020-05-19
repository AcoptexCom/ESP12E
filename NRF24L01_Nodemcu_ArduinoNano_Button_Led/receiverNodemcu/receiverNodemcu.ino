//Receiver Side Code for ESP8266 ESP-12E module
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const int led=0;//green led attached to D3 pin (GPIO0) of Nodemcu
// Hardware configuration
// Set up nRF24L01 radio on pins D4 & D2 of Nodemcu
RF24 radio(2, 4);//CE, CSN
const byte pipe[6] = "00001";// Radio pipe address for the 2 nodes to communicate.
int buttonState;

void setup() {
  Serial.begin(9600);//initialize serial communication at 9600 bps
 pinMode(led, OUTPUT);
  radio.begin();
  radio.openReadingPipe(0, pipe);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
   radio.read(&buttonState, sizeof(buttonState));
  if (buttonState == 1) {
   Serial.println("LED is ON");
   digitalWrite(led, HIGH);
 }
  else {
    digitalWrite(led, LOW);
    Serial.println("LED is OFF");
  }
 } 
 }
