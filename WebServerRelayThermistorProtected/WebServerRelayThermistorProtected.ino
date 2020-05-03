// Load necessary libraries
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
const double VCC = 3.3;             // NodeMCU on board 3.3v vcc
const double R2 = 10000;            // 10k ohm series resistor
const double adc_resolution = 1023; // 10-bit adc

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741;


// Replace with your network credentials
const char* ssid     = "TR-1";
const char* password = "1234";
const int webServerPort = 80;
String content;
// Set your Static IP address
IPAddress local_IP(192, 168, 1, 211);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 1, 0); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
// Set web server port number
ESP8266WebServer server(webServerPort);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output2State = "off";
String output1State = "off";
// Assign output variables to GPIO pins
const int output1 = 5;//IN1 aatached to Nodemcu D1(GPIO5) pin
const int output2 = 4;//IN2 attched to Nodemcu D2(GPIO4) pin

//Check if header is present and correct
bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentication Successful");
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin") { //To log in, please use : admin/admin
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }

  content = "<!DOCTYPE html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\">";
  content += "<style>html {font-family: Helvetica; display: inline-block;text-align: center;background:#9ECCFF}";
  content += ".log-form {background: #FF9549;margin-left:auto; margin-right:auto;width: 40%;}";
  content += "h2 {width: 100%;color: black;font-size: 25px;display: block;text-transform: uppercase;font-weight: 150;}";
  content += "input {display: block;margin-left:auto; margin-right:auto;width: 80%;font-size:15px}";
  content += ".btn {background-color: #195B6A; border: none; color: white; padding: 16px 40px;text-align: center;text-decoration: none; font-size: 20px; margin: 2px; cursor: pointer;}";
  content += ".forgot {color: red;position: relative;text-decoration: none;font-size: 15px;}";
  content += "</style></head>";
  content += "<body><div class=\"log-form\"><h2>ESP8266 WEB SERVER</h2>";
  content += "<form action='/login' method='POST'><input type=\"text\" title=\"username\" name='USERNAME' placeholder=\"username\" /><br><br>";
  content += "<input type=\"password\" title=\"username\" name='PASSWORD' placeholder=\"password\" /><br><br>";
  content += "<button type=\"submit\" name='SUBMIT' class=\"btn\">Login</button><br>You also can go <a class=\"forgot\"href='/inline'>here</a></form></div>"+ msg+"</body></html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentication is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  if (!is_authenticated()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  //Thermistor readings
  double Vout, Rth, temperature, temperature1, adc_value;
  adc_value = analogRead(A0);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;
  /*  Steinhart-Hart Thermistor Equation:
      Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
      where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
  temperature = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)), 3))));  // Temperature in kelvin
  temperature = temperature - 273.15;  // Temperature in degree celsius
  temperature1 = (temperature * 1.8) + 32; // Temperature in degree fahrenheight
  // Display the HTML web page
  // CSS to style the on/off buttons
  // Feel free to change the background-color and font-size attributes to fit your preferences
  content = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\">";
  content += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;background:#9ECCFF}";
  content += "table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }";
  content += ".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
  content += "th { padding: 12px; background-color: #FF9549; color: white; }";
  content += "tr { border: 1px solid #ddd; padding: 12px; }";
  content += "tr:hover { background-color: #0094FF; }";
  content += "td { border: none; padding: 12px; }";
  content += ".sensor { color:white; font-weight: bold; background-color: #0094FF; padding: 1px; }";
  content += ".button2 {background-color: #77878A;}</style></head>";
  // Web Page Heading
  content += "<body><h1>ESP8266 Web Server</h1><br>";
  content += "<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>";
  //Data from temperature sensor Celsius
  content += "<tr><td>Temperature Cels.</td><td><span class=\"sensor\">";
  content += temperature;
  content += " *C</span></td></tr>";
  //Data from temperature sensor Fahrenheight
  content += "<tr><td>Temperature Fahr.</td><td><span class=\"sensor\">";
  content += temperature1;
  content += " *F</span></td></tr></table>";
  // Display current state, and ON/OFF buttons for GPIO
  content += "<p>GPIO 4 - State " + output1State + "</p>";
  content += "<p>GPIO 5 - State " + output2State + "</p>";
  // If the outputState is off, it displays the ON button
  if (output1State == "off") {
    content += "<p><a href=\"/gpio4On\" ><button class=\"button\">ON</button></a></p>";
  } else {
    content += "<p><a href=\"/gpio4Off\"><button class=\"button button2\">OFF</button></a></p>";
  }
  if (output2State == "off") {
    content += "<p><a href=\"/gpio5On\" ><button class=\"button\">ON</button></a></p>";
  } else {
    content += "<p><a href=\"/gpio5Off\"t><button class=\"button button2\">OFF</button></a></p>";
  }
  content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";
  server.send(200, "text/html", content);
}

//no need authentication
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output2, OUTPUT);
  pinMode(output1, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output2, LOW);
  digitalWrite(output1, LOW);
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/login", handleLogin);

  server.on("/gpio5On", []() {
    //server.send(200, "text/html", content);
    Serial.println("GPIO 5 on");
    output2State = "on";
    digitalWrite(output2, HIGH);
    server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
  });
  server.on("/gpio5Off", []() {
    //server.send(200, "text/html", content);
    Serial.println("GPIO 5 off");
    output2State = "off";
    digitalWrite(output2, LOW);
    server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
  });

  server.on("/gpio4Off", []() {
    // server.send(200, "text/html", content);
    Serial.println("GPIO 4 off");
    output1State = "off";
    digitalWrite(output1, LOW);
    server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
  });
  server.on("/gpio4On", []() {
    //server.send(200, "text/html", content);
    Serial.println("GPIO 4 on");
    output1State = "on";
    digitalWrite(output1, HIGH);
    server.sendHeader("Location", "/");       // Add a header to respond with a new location for the browser to go to the home page again
    server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
  });

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentication");
  });

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

}
