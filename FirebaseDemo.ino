/*

  Extract & Install CH341SER

  Setup Board : 
    1. File - Preferences
    2. Additional Board Manager Urls :
    3. Paste http://arduino.esp8266.com/stable/package_esp8266com_index.json
    4. Tools - Board - Boards Manager
    5. Install esp8266 by ESP8266 Community - Version 3.1.2
    6. Tools - Upload Speed - 115200
  

  Install Library :
  ArduinoJson by Benoit Blanchon - Version 5.13.5
  ESP8266 Firebase by Rupak Poddar - Version 1.3.1
  TinyGPSPlus by Mikal Hart - Version 1.0.3

*/

#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

#define _SSID "....."          // Your WiFi SSID
#define _PASSWORD "....."      // Your WiFi Password
#define REFERENCE_URL "....."  // Your Firebase project reference url

// Example Url : https://monitoring-gps-kecepatan-rtdb.asia-southeast1.firebasedatabase.app/

static const int RXPin = D4, TXPin = D3; // Pin Neo 6M Gps
static const int Hall = D5; // Pin Hall Effect Magnetic Sensor
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
}

void displayInfo() {
  millis();
  if(gps.location.isValid()) {
  
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
      firebase.setInt("Monitoring/Count", count); // Bagian ini bisa di hapus
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
