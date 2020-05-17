//Receiver Side Code for Arduino Nano
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>
// Hardware configuration
// Set up nRF24L01 radio on pins D8 & D10 of  Arduino Nano
RF24 radio(8, 10);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);// initialise the LCD
float data[2];
const uint64_t pipe = 0xE8E8F0F0E1LL;// Radio pipe address for the 2 nodes to communicate.

void setup()
{
  Serial.begin(9600);//initialize serial communication at 9600 bps
  radio.begin();// Setup and configure rf radio
  radio.openReadingPipe(1, pipe);
  radio.startListening();
  lcd.begin (16, 2);    // define the LCD as 16 column by 2 rows
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH); // switch on the backlight
  lcd.setCursor(0, 0);
  lcd.print("   Welcome to        ");
  lcd.setCursor(0, 1);
  lcd.print(" Acoptex      ");
  delay(1000);
  lcd.clear();
}

void loop(void)
{
if (radio.available())
{
bool flag = false;
while (!flag)
{
radio.read(data, sizeof(data));

delay(500);
lcd.setCursor(0, 0);
lcd.print("Temp");
lcd.setCursor(0, 1);
lcd.print("Humidity");
lcd.setCursor(9, 0);
lcd.print(data[0]);
lcd.print(" C");
lcd.setCursor(9, 1);
lcd.print(data[1]);
lcd.print(" %");
delay(1000);
}
}
else
{
lcd.setCursor(0, 0);
lcd.print("No Radio Found");
delay(1000);
lcd.clear();
}  
}
