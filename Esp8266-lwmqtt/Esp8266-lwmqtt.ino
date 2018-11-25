/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "esp8266_mqtt.h"
#include <ArduinoJson.h>

// Memory pool for JSON object tree.
//
// Inside the brackets, 200 is the size of the pool in bytes.
// Don't forget to change this value to match your JSON document.
// Use arduinojson.org/assistant to compute the capacity.
StaticJsonBuffer<200> jsonBuffer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupWifi();
  pinMode(LED_BUILTIN, OUTPUT);
  startMQTT();  
}

unsigned long lastMillis = 0;
void loop() {
  mqttClient.loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient.connected()) {
    connect();
  }

  // Create the root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  JsonObject& root = jsonBuffer.createObject();

  root["tipo"] = "luminosidad";
  root["valor"] = 3;

  char JSONmessageBuffer[100];
  root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  
  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();    
    publishTelemetry(JSONmessageBuffer);
  }
}
