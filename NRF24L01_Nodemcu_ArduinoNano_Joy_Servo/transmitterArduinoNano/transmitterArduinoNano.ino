//Transmitter Side Code for Arduino Nano
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
const int joystickVRxPin = A0;//Joystick VRx pin attached to A0 pin of ArduinoNano
// Hardware configuration
// Set up nRF24L01 radio on pins D8 & D10 of  Arduino Nano
RF24 radio(8, 10);
const byte pipe[6] = {"00002"};// Radio pipe address for the 2 nodes to communicate.

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  radio.stopListening();
}

void loop()
{
  int potValue = analogRead(joystickVRxPin);
  int angleValue = map(potValue, 516, 1023, 0, 180);
  radio.write(&angleValue, sizeof(angleValue));
  Serial.println(angleValue);
  delay(1000);
}
