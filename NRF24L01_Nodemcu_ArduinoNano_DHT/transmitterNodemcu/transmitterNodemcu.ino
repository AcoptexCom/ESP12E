//Transmitter Side Code for ESP8266 ESP-12E module
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"

#define DHTPIN 0     //DHT sensor attached to Nodemcu D3 pin (GPIO0)
// Uncomment which type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

float temperature[2];
 
RF24 radio(2, 4);
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
void setup(void) {
  Serial.begin(9600);
dht.begin();
radio.begin();
radio.openWritingPipe(pipe);
}
 
void loop(void)
{
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.print("Sensor failed");
    return;
  }
 
  Serial.print("Temperature Celsius: ");
  Serial.println(t);
  Serial.print("Temperature Fahrenheit: ");
  Serial.println(f);
  Serial.print("Humidity: ");
  Serial.println(h);

temperature[0] = t;
temperature[1] = h;
radio.write(temperature, sizeof(temperature));
delay(500);
}
