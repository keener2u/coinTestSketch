/**************************
 * Mashup of the ThingM Example code from SparkFun and a sketch to build a roomba virtual wall. 
 * Both modified to work with the Coin. 
 * Please change your PSK password.
 */
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#define ESP8266_LED D4
#define ESP8266_LED2 D8

//////////////////////
// WiFi Definitions //
//////////////////////
const char WiFiAPPSK[] = "coinpass";
bool runWall=false;
WiFiServer server(80);
const uint16_t kIrLed = D2;
IRsend irsend(kIrLed);


void setup() 
{
  initHardware();
  setupWiFi();
  irsend.begin();
  server.begin();

}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    if(runWall)
      roomba_send(137);
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
  // request type (read/set) and value if set.
  if (req.indexOf("/led/0") != -1)
    val = 0; // Will write LED low
  else if (req.indexOf("/led/1") != -1)
    val = 1; // Will write LED high
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print pin reads
 
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO5 according to the request
  if (val >= 0){
   digitalWrite(ESP8266_LED2, val);
   digitalWrite(ESP8266_LED, val);
   digitalWrite(kIrLed, val);
  }

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "LED is now ";
    s += (val)?"on":"off";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values: *for later expansion
    runWall=!(runWall);
    s += "Roomba Wall is ";
    s += (runWall)?"on":"off";
  }
  else
  {
    s += "Invalid Request.<br> Try /led/1, /led/0, or /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
 
}

void setupWiFi()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
    String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "Coin_" + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(ESP8266_LED, OUTPUT);
  pinMode(ESP8266_LED2, OUTPUT);
  digitalWrite(ESP8266_LED, LOW);
  digitalWrite(ESP8266_LED2, HIGH);
  
  
  
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}

void roomba_send(int code) 
{
  Serial.print("Sending Roomba code ");
  Serial.print(code);
  int length = 8;
  uint16_t raw[length*8];
  unsigned int one_pulse = 3000;
  unsigned int one_break = 1000;
  unsigned int zero_pulse = one_break;
  unsigned int zero_break = one_pulse;

  int arrayposition = 0;
  // Serial.println("");
  for (int counter = length-1; counter >= 0; --counter) {
    if(code & (1<<counter)) {
      // Serial.print("1");
      raw[arrayposition] = one_pulse;
      raw[arrayposition+1] = one_break;
    } 
    else {
      // Serial.print("0");
      raw[arrayposition] = zero_pulse;
      raw[arrayposition+1] = zero_break;
    }
    arrayposition = arrayposition + 2;
  } 
  for (int i = 0; i < 3; i++) {
    irsend.sendRaw(raw, 15, 38);
    delay(50);
  }
  Serial.println("");

  
  Serial.print("Raw timings:");
   for (int z=0; z<length*2; z++) {
   Serial.print(" ");
   Serial.print(raw[z]);
   }
   Serial.print("\n\n");
   
}
