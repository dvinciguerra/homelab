#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <locale.h>
#include "NodeMCU.h"

// constants
const char *kWifiSSID = "WIFI_SSID";
const char *kWifiPassword = "WIFI_PASSWORD";

const char *kMqttHost = "MQTT_HOST";
const int kMqttPort = 1883;
const char *kMqttClient = "esp8266-xxx";
const char *kMqttTopicPub = "sensors/foo";

// globals
WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);

namespace serial {
void NewLine() { Serial.println(""); }
}  // namespace serial

namespace wifi {
void Reconnect() {
  WiFi.begin(kWifiSSID, kWifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  serial::NewLine();
  Serial.println("[Wifi] Connected!");
  Serial.print("[Wifi] Local IP address: ");
  Serial.println(WiFi.localIP());
}

void Setup() {
  serial::NewLine();
  Serial.println((String) "[Wifi] Connecting to " + kWifiSSID);

  Reconnect();
}
}  // namespace wifi

namespace mqtt {
void Setup() { mqtt_client.setServer(kMqttHost, kMqttPort); }

void Connect() {
  while (!mqtt_client.connected()) {
    if (!mqtt_client.connect(kMqttClient)) {
      Serial.println("[MQTT] Connection failed (attempting in 5 sec)...");
      delay(5000);

      continue;
    }

    Serial.println("[MQTT] Connected!");
    return;
  }
}
}  // namespace mqtt

void setup() {
  // configuration
  setlocale(LC_ALL, "");
  Serial.begin(9600);

  delay(10000);

  // initializations
  wifi::Setup();
  mqtt::Setup();

  delay(5000);
}

void loop() {
  String payload = "{\"value\": \"foo\"}";

  if (!mqtt_client.connected()) mqtt::Connect();

  Serial.println("[MQTT] Publishing payload...");
  mqtt_client.publish(kMqttTopicPub, payload.c_str());

  delay(10000);
}
