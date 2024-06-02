/*
  Make sure your Firebase project's '.read' and '.write' rules are set to 'true'. 
  Ignoring this will prevent the MCU from communicating with the database. 
  For more details- https://github.com/Rupakpoddar/ESP8266Firebase 
*/

#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#define _SSID "Hp gw"          // Your WiFi SSID
#define _PASSWORD "01020304"      // Your WiFi Password
#define REFERENCE_URL "https://monitoring-gps-kecepatan-default-rtdb.asia-southeast1.firebasedatabase.app/"  // Your Firebase project reference url

static const int RXPin = D4, TXPin = D3;
static const int Hall = D5;
static const uint32_t GPSBaud = 9600;
unsigned long waktuAwal;
int count = 0, rps = 0;

TinyGPSPlus gps;
Firebase firebase(REFERENCE_URL);
SoftwareSerial ss(TXPin, RXPin);

void setup() {
  Serial.begin(115200);
  pinMode(Hall, INPUT_PULLUP);
  ss.begin(GPSBaud);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);

//================================================================//
//================================================================//

  // Examples of setting String, integer and float values.
//  firebase.setString("Example/setString", "It's Working");
//  firebase.setInt("Example/setInt", 123);
//  firebase.setFloat("Example/setFloat", 45.32);
//
//  // Examples of pushing String, integer and float values.
//  firebase.pushString("push", "Hello");
//  firebase.pushInt("push", 789);
//  firebase.pushFloat("push", 89.54);
//
//  // Example of getting a String.
//  String data1 = firebase.getString("Example/setString");
//  Serial.print("Received String:\t");
//  Serial.println(data1);
//
//  // Example of data deletion.
//  firebase.deleteData("Example");
}

void displayInfo() {
  if(gps.location.isValid()) {
    millis();

    if(millis() - waktuAwal > 1000) {
      rps = count;
      count = 0;
      String kecepatan = String(rps);
      kecepatan += " ";
      kecepatan += "rps";
      firebase.setString("Monitoring/Kecepatan", kecepatan);
      waktuAwal = millis();
    }

    if(digitalRead(Hall) == 0) {
      count++;
      firebase.setInt("Monitoring/Count", count);
      while(digitalRead(Hall) == 0){}
    }

    
    float currentLat = gps.location.lat();
    float currentLng = gps.location.lng();
    
    String lokasi = "https://maps.google.com?q=";
    lokasi += String(currentLat, 6);
    lokasi += ",";
    lokasi += String(currentLng, 6);
    firebase.setString("Monitoring/Lokasi", lokasi);
    
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void loop() {
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}
