#include <WiFi.h>
#include <WebServer.h>
#include "homeweb.h"
#include "led.h"
#include "Firmware.h"

const char* ssid = "Mindbase_Corp_CMI_2.4G";
const char* password = "mindb@se963";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("WiFi connected: ");
  Serial.println(WiFi.localIP());

  setupWebServer();  // จาก homeweb.h
  setupWebLed();     // จาก led.h
}

void loop() {
  handleWebServer(); // จาก homeweb.h
  updateLedEffect(); // จาก led.h
}


