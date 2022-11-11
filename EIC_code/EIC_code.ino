#include <DHT.h>;
#include <ESP8266WiFi.h>;

const int trigPin = D5;
const int echoPin = D6;
const int relay = D5;

long duration;
int height;
unsigned long StartTime = 1;
unsigned long Interval = 86400000;

String apiKey = "2JVX15OU51JFZ3ZA";
const char* server = "api.thingspeak.com";

const char *ssid = "OPPO Reno5 5G";
const char *pass = "fbV3tj57";
#define DHTPIN D2
DHT dht(DHTPIN, DHT22);
WiFiClient client;
void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  delay(10);

  pinMode(relay, OUTPUT);
  
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  
  digitalWrite(D1, HIGH);
  digitalWrite(D3, LOW);
  delay(1000);

  dht.begin();

  Serial.println("Trying to Connect with ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  if (millis()>StartTime){
    StartTime = millis()+Interval;
    digitalWrite(relay, HIGH);
    delay(12000);
    digitalWrite(relay, LOW);
  }
 
  duration = pulseIn(echoPin, HIGH);

  height = (duration * 0.034 / 2);

  Serial.print("Height: ");
  Serial.println(height,1);
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (client.connect(server, 80))
  {
    Serial.print("IN");
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(height);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    client.flush();
    Serial.println(postStr);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" degrees Celcius, Humidity: ");
    Serial.print(h);
    Serial.println("%.");
    Serial.println("Data has been sussecfully sent to Thingspeak.");
  }
  client.stop();

  Serial.println("Waiting...");

  delay(60000);
}
