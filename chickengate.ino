/* 
 * Project : ChickenGate
 * FabFarm
 * 
 * Free Licence
 * 25-02-2020
 * 
 * Author : Fabien <fabien@zaplana.fr>
*/

// Include the ESP8266 Library
#include <ESP8266WiFi.h>

String codeVersion = "ChickenGate Version 0.000 - FabFarm";

// WiFi Router Login
const char* SSID = "fabfarm-ele-container";
const char* password = "imakestuff";

// Setup GPIO2 for LED TEST
int pinGPIO2 = 2; // To control LED
int doorStatus = 0; //0=off,1=Open,2=Close

// Create the Web Server
WiFiServer WebServer(80);
// Web Client
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println(codeVersion);

  // Setup the GPIO2 LED Pin
  pinMode(pinGPIO2, OUTPUT);
  digitalWrite(pinGPIO2, LOW);
  doorStatus = 0;

  // Connect to WiFi network
  Serial.println();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(SSID);
  WiFi.begin(SSID, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  // Start the Web Server
  WebServer.begin();
  Serial.println("Web Server started");

  // Print the IP address
  Serial.print("You can connect to the ESP8266 at this URL: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Check if a user has connected
  client = WebServer.available();
  if (!client) {//restart loop
    return;
  }

  // Wait until the user sends some data
  Serial.println("New User");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r\n');
  Serial.println(request);
  client.flush();

  // Process the request:
  if (request.indexOf("/DOOR=OPEN") != -1) {
    analogWrite(pinGPIO2, 1023);
    doorStatus = 1;
  }
  if (request.indexOf("/DOOR=OFF") != -1) {
    analogWrite(pinGPIO2, 0);
    doorStatus = 0;
  }
  if (request.indexOf("/DOOR=CLOSE") != -1) {
    analogWrite(pinGPIO2, 512);
    doorStatus = 2;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>Chicken Gate</title>");
  client.println("</head>");
  client.println("<body>");
  client.println("<a href=\"/\">Refresh Status</a>");
  client.println("</br></br>");

  //check the LED status
  if (doorStatus == 0) {
    client.print("Door is not moving</br>");
    client.println("<a href=\"/DOOR=OPEN\">Open the door</a></br>");
    client.println("<a href=\"/DOOR=CLOSE\">Close the door</a></br>");
  } else if (doorStatus == 1) {
    client.print("Door is opening</br>");
    client.println("<a href=\"/DOOR=OFF\">Stop move the door</a></br>");
    client.println("<a href=\"/DOOR=CLOSE\">Close the door</a></br>");
  } else if (doorStatus == 2) {
    client.print("Door is Closing</br>");
    client.println("<a href=\"/DOOR=OFF\">Stop move the door</a></br>");
    client.println("<a href=\"/DOOR=OPEN\">Open the door</a></br>");
  }

  client.println("</br>");
  client.println("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("User disconnected");
  Serial.println("");

}
