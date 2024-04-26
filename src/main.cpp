#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <list>
using namespace std;

const char* ssid = "wifi";
const char* password = "password";

// Server information
const char* serverName = "http://192.168.1.117:8080";

struct PinData {
  String id;
  String value;
  String pinName;
};

void printPinList(const list<PinData>& pinList) { // Function to print the list
  for (const PinData& pin : pinList) {  // Use a reference to avoid unnecessary copies
    Serial.print("Pin ID: ");
    Serial.println(pin.id);
    Serial.print("Value: ");
    Serial.println(pin.value);
    Serial.print("Pin Name: ");
    Serial.println(pin.pinName);
  }
}

list<PinData> pinListMain;

// Function for GET request
void makeGetRequest(String endpoint) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Create a WiFiClient object
    HTTPClient http;

    String url = serverName + endpoint;
    http.begin(client, url); 

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      

      // Deserialize JSON using ArduinoJson
      JsonDocument doc; // Using the unified JsonDocument
      deserializeJson(doc, response);
      DeserializationError error = deserializeJson(doc, response);

      Serial.println(doc.size());

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      // Create a list to hold PinData
      list<PinData> pinList;

      // Extract pin data into the list 
      JsonArray pinsArray = doc.as<JsonArray>();
      Serial.println(pinsArray.size());

      for (JsonObject pinObj : pinsArray) {
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
  } else {
    Serial.println("WiFi not connected.");
  }
}

// Function for POST request
void makePostRequest(String endpoint, String postData) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Create a WiFiClient object
    HTTPClient http;


    String url = serverName + endpoint;
    http.begin(client, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);

    // Handles Response code
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

  } else {
    Serial.println("WiFi not connected.");
  }
}

void setPins(){

  for (const PinData& pin : pinListMain) {  // Use a reference to avoid unnecessary copies
    Serial.print("Pin ID: ");
    Serial.println(pin.id);
    Serial.print("Value: ");
    Serial.println(pin.value);
    Serial.print("Pin Name: ");
    Serial.println(pin.pinName);

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
    case 5:
      analogWrite(D4, pin.value.toInt());
      break;
    case 8:
      analogWrite(D5, pin.value.toInt());
      break;
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


void setup() {
  Serial.begin(9600);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");


  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(A0, INPUT);

  
}

void loop() {
  
  makeGetRequest("/getboardjson");  // GET request to /getboardjson endpoint

  delay(50);

  setPins();
  
}
