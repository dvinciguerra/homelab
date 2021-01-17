#include <locale.h>
#include "DHT.h"
#include "NodeMCU.h"

#ifndef SENSOR_TYPE
#define SENSOR_TYPE DHT11
#endif /* ifndef SENSOR_TYPE */

#ifndef SENSOR_PIN
#define SENSOR_PIN D2
#endif /* ifndef SENSOR_PIN */

// constants
const int kSensorStartDelayTime = 1000;
const int kSensorReadDelayTime = 2000;

namespace serial {
void NewLine() { Serial.println(""); }
} // namespace serial

namespace humidity {
void Print(float value) {
  Serial.print((String) "\u2601 Humidity: " + value + " %t  ");
}
} // namespace humidity

namespace temperature {
void Print(float value) {
  Serial.print((String) "\u263C Temperature: " + value + " ºC");
}
} // namespace temperature

// globals
DHT dht(SENSOR_PIN, SENSOR_TYPE);

void setup() {
  // configurations
  setlocale(LC_ALL, "");
  Serial.begin(9600);

  // initializations
  Serial.println("[DHT Sensor] Starting sensor...");
  dht.begin();
  delay(kSensorStartDelayTime);
}

void loop() {
  float humidity_value = dht.readHumidity();
  float temperature_value = dht.readTemperature();

  if (isnan(temperature_value) || isnan(humidity_value)) {
    Serial.println("[DHT Sensor] Unable to read sensor data!");
    return;
  }

  humidity::Print(humidity_value);
  temperature::Print(temperature_value);
  serial::NewLine();

  // wait 2 secs to read sensor again
  delay(kSensorReadDelayTime);
}
