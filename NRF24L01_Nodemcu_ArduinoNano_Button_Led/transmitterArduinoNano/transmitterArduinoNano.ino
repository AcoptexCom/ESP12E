//Transmitter Side Code for Arduino Nano
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Hardware configuration
// Set up nRF24L01 radio on pins D8 & D10 of  Arduino Nano
const int button = 4;//momentary switch attached to D4 pin of ArduinoNano
int buttonState;
RF24 radio(8, 10);
const byte pipe[6] = {"00001"};// Radio pipe address for the 2 nodes to communicate.

void setup()
{
  Serial.begin(9600);
  pinMode(button, INPUT);
  radio.begin();
  radio.openWritingPipe(pipe);
  radio.stopListening();

}

void loop()
{
  buttonState = digitalRead(button);
  Serial.print(buttonState);
  radio.write(&buttonState, sizeof(buttonState));
  delay(1000);
}
