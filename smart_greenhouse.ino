#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// ================= WIFI =================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ================= FIREBASE =================
#define API_KEY "AIzaSyCcCKGnsybwjzJa1JQpDCLoxwdttpiqhtY"
#define DATABASE_URL "https://iot-esp32-grace-default-rtdb.asia-southeast1.firebasedatabase.app"

// LOGIN FIREBASE
#define USER_EMAIL "gracefristiani13@gmail.com"
#define USER_PASSWORD "Graceee13_"

// ================= SENSOR =================
#define LDR_PIN 34
#define DHT_PIN 15
#define PIR_PIN 27
#define DHTTYPE DHT22

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
DHT dht(DHT_PIN, DHTTYPE);

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  dht.begin();

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase Authenticated");
}

// ================= LOOP =================
void loop() {
  int light = map(analogRead(LDR_PIN), 4095, 0, 100);
  light = constrain(light, 0, 100);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool motion = digitalRead(PIR_PIN);

  String path = "/iot/sensor";

  if (Firebase.ready()) {
    Firebase.RTDB.setInt(&fbdo, path + "/light", light);
    Firebase.RTDB.setFloat(&fbdo, path + "/temperature", temperature);
    Firebase.RTDB.setFloat(&fbdo, path + "/humidity", humidity);
    Firebase.RTDB.setBool(&fbdo, path + "/motion", motion);
  }

  Serial.println("Data sent (authenticated)");
  delay(3000);
}
