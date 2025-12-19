#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DHT.h"

// ================= WIFI =================
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ================= FIREBASE =================
#define API_KEY "AIzaSyCcCKGnsybwjzJa1JQpDCLoxwdttpiqhtY"
#define DATABASE_URL "https://iot-esp32-grace-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "gracefristiani13@gmail.com"
#define USER_PASSWORD "Graceee13_"

// ================= SENSOR PIN =================
#define LDR_PIN 34      // ADC (input only)
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
  delay(100);

  pinMode(PIR_PIN, INPUT);
  dht.begin();

  // ===== WIFI CONNECT =====
  Serial.print("Connecting WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // ===== FIREBASE CONFIG =====
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase ready\n");
}

// ================= LOOP =================
void loop() {
  // ===== LDR =====
  int rawLdr = analogRead(LDR_PIN);
  int light = map(rawLdr, 4095, 0, 100);
  light = constrain(light, 0, 100);

  // ===== DHT22 =====
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // ===== PIR =====
  bool motion = digitalRead(PIR_PIN);

  // ===== SEND TO FIREBASE =====
  String path = "/iot/sensor";

  Firebase.RTDB.setInt(&fbdo, path + "/light", light);
  Firebase.RTDB.setFloat(&fbdo, path + "/temperature", temperature);
  Firebase.RTDB.setFloat(&fbdo, path + "/humidity", humidity);
  Firebase.RTDB.setBool(&fbdo, path + "/motion", motion);

  // ===== SERIAL MONITOR =====
  Serial.println("===== SENSOR DATA =====");
  Serial.printf("Light       : %d %%\n", light);
  Serial.printf("Temperature : %.2f °C\n", temperature);
  Serial.printf("Humidity    : %.2f %%\n", humidity);
  Serial.printf("Motion      : %s\n", motion ? "DETECTED" : "SAFE");
  Serial.println("=======================\n");

  delay(3000);
}
