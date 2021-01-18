#include "NodeMCU.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <locale.h>

// constants
const char *kWifiSSID = "WIFI_NAME";
const char *kWifiPassword = "WIFI_PASSWORD";

const unsigned int kSensorStartDelayTime = 1000;
const unsigned int kSensorReadDelayTime = 2000;

const String kSensorMetricsApiHost = "http://metrics.local:4000/";

namespace serial {
void NewLine() { Serial.println(""); }
} // namespace serial

// globals
unsigned long last_time = 0;

namespace wifi {
void Reconnect() {
  unsigned int attempt = 0;

  WiFi.begin(kWifiSSID, kWifiPassword);

  Serial.println("[Wifi] Reconnecting...");
  while (WiFi.status() != WL_CONNECTED) {
    if (attempt > 10)
      return;

    delay(500);
    Serial.print(".");

    attempt += 1;
  }

  serial::NewLine();
  Serial.print("[Wifi] Local IP address: ");
  Serial.println(WiFi.localIP());
}
} // namespace wifi

void setup() {
  // configurations
  setlocale(LC_ALL, "");
  Serial.begin(9600);

  // initializations
  Serial.println("[Wifi] Starting...");
  WiFi.begin(kWifiSSID, kWifiPassword);

  Serial.println("[Wifi] Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  serial::NewLine();
  Serial.println(WiFi.localIP());

  delay(kSensorStartDelayTime);
}

void loop() {
  HTTPClient http;
  String api_host = kSensorMetricsApiHost + "?temperature=24.37";

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Wifi] Wifi disconnected!");
    wifi::Reconnect();
  }

  http.begin(api_host.c_str());
  int http_response_code = http.GET();

  if (http_response_code > 0) {
    Serial.println((String) "[HTTP] response code: " + http_response_code);

    String http_payload = http.getString();
    Serial.println((String) "[HTTP] Response payload: " + http_payload);
  }

  http.end();

  delay(kSensorReadDelayTime);
}
