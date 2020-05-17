//Receiver Side Code for Arduino Nano
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>
RF24 radio(8, 10);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);// initialise the LCD
float temperature[2];
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup()
{
  Serial.begin(9600);
  radio.begin();
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
bool done = false;
while (!done)
{
radio.read(temperature, sizeof(temperature));

delay(500);
lcd.setCursor(0, 0);
lcd.print("Temp");
lcd.setCursor(0, 1);
lcd.print("Humidity");
lcd.setCursor(9, 0);
lcd.print(temperature[0]);
lcd.print(" C");
lcd.setCursor(9, 1);
lcd.print(temperature[1]);
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
