#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <list>
#include <Wire.h> // I2C library
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Or reset pin if applicable
using namespace std;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char *ssid = "Emre2.4";
const char *password = "Emre360111008";

// Server information
const char *serverName = "http://192.168.1.117:8080";

struct PinData
{
  String id;
  String value;
  String pinName;
};

void printPinList(const list<PinData> &pinList)
{ // Function to print the list
  for (const PinData &pin : pinList)
  { // Use a reference to avoid unnecessary copies
    Serial.print("Pin ID: ");
    Serial.println(pin.id);
    Serial.print("Value: ");
    Serial.println(pin.value);
    Serial.print("Pin Name: ");
    Serial.println(pin.pinName);
  }
}

list<PinData> pinListMain;
String dateMain; 

// Function for GET request
void makeGetRequest(String endpoint)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client; // Create a WiFiClient object
    HTTPClient http;

    String url = serverName + endpoint;
    http.begin(client, url);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
      String response = http.getString();

      // Deserialize JSON using ArduinoJson
      JsonDocument doc; // Using the unified JsonDocument
      deserializeJson(doc, response);
      DeserializationError error = deserializeJson(doc, response);

      if (error)
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      // Create a list to hold PinData
      list<PinData> pinList;

      // Extract pin data into the list
      JsonArray pinsArray = doc["boardList"].as<JsonArray>();
      dateMain = doc["time"].as<String>();

      for (JsonObject pinObj : pinsArray)
      {
        PinData pin;
        pin.id = pinObj["id"].as<String>();
        pin.value = pinObj["value"].as<String>();
        pin.pinName = pinObj["pinName"].as<String>();
        pinList.push_back(pin);
      }

      // (You can now access the pin data using the pinList)
      pinListMain = pinList;
    }

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected.");
  }
}

// Function for POST request
void makePostRequest(String endpoint, String postData)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client; // Create a WiFiClient object
    HTTPClient http;

    String url = serverName + endpoint;
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);

    // Handles Response code
    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
  }
  else
  {
    Serial.println("WiFi not connected.");
  }
}

void setPins()
{

  for (const PinData &pin : pinListMain)
  { // Use a reference to avoid unnecessary copies

    switch (pin.id.toInt())
    {
    case 1:
      analogWrite(D0, pin.value.toInt());
      break;
    case 2:
      analogWrite(D1, pin.value.toInt());
      break;
    case 3:
      analogWrite(D2, pin.value.toInt());
      break;
    case 4:
      analogWrite(D3, pin.value.toInt());
      break;
    // case 5:
    // analogWrite(D4, pin.value.toInt());
    // break;
    // case 8:
    // analogWrite(D5, pin.value.toInt());
    // break;
    case 9:
      analogWrite(D6, pin.value.toInt());
      break;
    case 10:
      analogWrite(D7, pin.value.toInt());
      break;
    case 11:
      analogWrite(D8, pin.value.toInt());
      break;

    default:
      break;
    }
  }
}

void displayPins()
{
  int counter = 0;
  const int maxPinsPerColumn = 3;
  int lineSpacing = 11; // Example spacing between lines

  display.clearDisplay();

  for (const PinData &pin : pinListMain)
  {
    int column = counter % maxPinsPerColumn; // Determines left (0) or right (1) column
    int line = counter / maxPinsPerColumn;   // Line number within the column
    // FIX ABOVE
    display.setCursor(column * 42, line * lineSpacing); // Calculate position

    string cstr = pin.pinName.c_str();
    if (cstr.find("D") != string::npos)
    {
      display.println(pin.pinName + "-" + pin.value);
      counter++;
    }
  }
  display.setCursor(0, 32);
  display.println(dateMain.substring(0,10));
  display.println(dateMain.substring(11,19) + "  " + dateMain.substring(20, 24));
  display.display();
}

void setup()
{
  Serial.begin(9600);
  delay(10);
  Wire.begin(D4, D5);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(1); // For minimum brightness
  display.println(ssid);
  display.println(password);
  display.println();
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  display.println();
  display.print("Wifi connected");
  display.display();

  Serial.println("");
  Serial.println("WiFi connected");

  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  // pinMode(D4, OUTPUT);
  // pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(A0, INPUT);

  delay(500);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(0x01); // For minimum brightness
  delay(100);
}

void loop()
{

  makeGetRequest("/getboardwithtime");
  displayPins();
  

  setPins();
}
