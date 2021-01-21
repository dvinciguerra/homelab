#include "DHT.h"
#include "NodeMCU.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <locale.h>

#ifndef SENSOR_TYPE
#define SENSOR_TYPE DHT11
#endif /* ifndef SENSOR_TYPE */

#ifndef SENSOR_PIN
#define SENSOR_PIN D2
#endif /* ifndef SENSOR_PIN */

// constants
const char *kWifiSSID = "WIFI_SSID";
const char *kWifiPassword = "WIFI_PASSWORD";

const char *kMqttHost = "MQTT_HOST";
const int kMqttPort = 1883;
const char *kMqttClient = "esp8266";
const char *kMqttTopicPub = "MQTT_TOPIC_PUB";

const int kSensorStartDelayTime = 1000;
const int kSensorReadDelayTime = 2000;

// globals
WiFiClient esp_client;
PubSubClient mqtt_client(esp_client);
DHT dht(SENSOR_PIN, SENSOR_TYPE);

namespace serial {
void NewLine() { Serial.println(""); }
} // namespace serial

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
} // namespace wifi

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
} // namespace mqtt

namespace dht_sensor {
void Setup() {
  Serial.println("[DHT Sensor] Starting sensor...");
  dht.begin();
  delay(kSensorStartDelayTime);
}
} // namespace dht_sensor

void setup() {
  // configuration
  setlocale(LC_ALL, "");
  Serial.begin(9600);

  delay(10000);

  // initializations
  wifi::Setup();
  mqtt::Setup();
  dht_sensor::Setup();

  delay(5000);
}

void loop() {
  float humidity_value = dht.readHumidity();
  float temperature_value = dht.readTemperature();

  if (isnan(temperature_value) || isnan(humidity_value)) {
    Serial.println("[DHT Sensor] Unable to read sensor data!");
    return;
  }

  if (!mqtt_client.connected())
    mqtt::Connect();

  Serial.println((String) "[MQTT] Sending humidity: " + humidity_value + "...");

  String humidity_topic = String(kMqttTopicPub) + "/humidity";
  String humidity_payload = "{\"value\": " + String(humidity_value) + "}";
  mqtt_client.publish(humidity_topic.c_str(), humidity_payload.c_str());

  Serial.println((String) "[MQTT] Sending temperature: " + temperature_value +
                 "...");

  String temperature_topic = String(kMqttTopicPub) + "/temperature";
  String temperature_payload = "{\"value\": " + String(temperature_value) + "}";
  mqtt_client.publish(temperature_topic.c_str(), temperature_payload.c_str());

  delay(10000);

  // wait 2 secs to read sensor again
  delay(kSensorReadDelayTime);
}
