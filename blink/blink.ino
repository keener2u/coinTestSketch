#define ESP8266_LED D4
#define ESP8266_LED2 D8
#define ESP8266_IR D3

void setup()
{
  pinMode(ESP8266_LED, OUTPUT);
  pinMode(ESP8266_LED2, OUTPUT);
  pinMode(ESP8266_IR, OUTPUT);
  
}

void loop()
{
  digitalWrite(ESP8266_LED, HIGH);
  digitalWrite(ESP8266_LED2, LOW);
  digitalWrite(ESP8266_IR, HIGH);
  delay(500);
  digitalWrite(ESP8266_LED, LOW);
  digitalWrite(ESP8266_LED2, HIGH);
  digitalWrite(ESP8266_IR, LOW);
  delay(500);
}
