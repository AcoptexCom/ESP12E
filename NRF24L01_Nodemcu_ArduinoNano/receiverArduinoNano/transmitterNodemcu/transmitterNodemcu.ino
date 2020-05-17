//Transmitter Side Code for ESP8266 ESP-12E module

#include <RH_NRF24.h>
#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "TR-1";
const char* password = "1234";

RH_NRF24 nrf24(2,4);
 
void setup() 
{
  Serial.begin(9600);
  while (!Serial);      
  if (!nrf24.init())
    Serial.println("initialization failed");
  if (!nrf24.setChannel(1))
    Serial.println("Channel set failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("RF set failed");    

   Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  configTime(-10800, 0, "0.europe.pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}
 
void loop()
{
  Serial.println("Sending data to receiver");
  time_t now = time(nullptr);
  String timestring = String(ctime(&now));
  uint8_t dataArray[timestring.length()];
  timestring.getBytes(dataArray,timestring.length());
  nrf24.send(dataArray,sizeof(dataArray));
  nrf24.waitPacketSent();
  delay(1000);
  uint8_t buf1[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf1);
    if(nrf24.recv(buf1, &len))
    {
      Serial.print("Temperture: ");
      Serial.println((char*)buf1);
     
    }
    else
    {
      Serial.println("recv failed");
    }
}
