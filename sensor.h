#ifndef SENSOR_H
#define SENSOR_H
#define DHTPIN 4
#define DHTTYPE DHT11

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <FastLED.h>
#include <DHT.h>

const int relayPins[4] = {15, 16, 17, 18}; // กำหนดขา GPIO สำหรับ relay
bool relayState[4] = {false, false, false, false}; // false = OFF (HIGH), true = ON (LOW)

DHT dht(DHTPIN, DHTTYPE);
unsigned long lastDHTSend = 0;
const unsigned long DHTInterval = 500; // 2 seconds

extern WebServer server;

WebSocketsServer webSocket = WebSocketsServer(81);

const int buttonPin = 2;
int buttonState = HIGH;
int lastButtonState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head><meta charset="UTF-8"><title>WebSocket Button</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Kanit:wght@400;700&display=swap');

body {
  font-family: 'Kanit', sans-serif;
  background: linear-gradient(135deg, #0f2027, #203a43, #2c5364);
  color: #fff;
  margin: 0;
  padding: 20px;
  text-align: center;
  min-height: 100vh;
}

h1 {
  font-size: 2em;
  margin: 30px 0 15px;
  text-shadow: 2px 2px 6px rgba(0,0,0,0.5);
}

a {
  color: #fff;
  background: rgba(255,255,255,0.08);
  padding: 10px 20px;
  border-radius: 10px;
  text-decoration: none;
  font-weight: bold;
  margin-top: 30px;
  display: inline-block;
  transition: background 0.3s, transform 0.2s;
}
a:hover {
  background: rgba(255,255,255,0.2);
  transform: scale(1.05);
}

.btn {
  padding: 14px 28px;
  font-size: 1.2em;
  border: none;
  border-radius: 12px;
  color: white;
  font-weight: bold;
  box-shadow: 0 8px 20px rgba(0,0,0,0.4);
  transition: transform 0.2s, background 0.3s;
}
.btn:hover {
  transform: scale(1.06);
}

.green {
  background: linear-gradient(45deg, #28a745, #3ed157);
}
.gray {
  background: linear-gradient(45deg, #6c757d, #999);
}

.toggle-group {
  display: flex;
  justify-content: center;
  gap: 25px;
  flex-wrap: wrap;
  margin-top: 30px;
}

/* Toggle Switch Styles */
.switch {
  position: relative;
  display: inline-block;
  width: 70px;
  height: 40px;
}
.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}
.slider {
  position: absolute;
  cursor: pointer;
  top: 0; left: 0; right: 0; bottom: 0;
  background-color: #555;
  transition: 0.4s;
  border-radius: 34px;
  box-shadow: inset 0 0 10px rgba(0,0,0,0.3);
}
.slider:before {
  position: absolute;
  content: "";
  height: 30px;
  width: 30px;
  left: 5px;
  bottom: 5px;
  background-color: white;
  transition: 0.4s;
  border-radius: 50%;
  box-shadow: 0 2px 6px rgba(0,0,0,0.3);
}
input:checked + .slider {
  background: linear-gradient(45deg, #28a745, #3ed157);
  box-shadow: 0 0 10px #28a745;
}
input:checked + .slider:before {
  transform: translateX(30px);
}
</style>
<script>
let socket = new WebSocket("ws://" + location.hostname + ":81/");
socket.onopen = function() {
  console.log("WebSocket connected");
};
socket.onmessage = function(event) {
  console.log("Received:", event.data);

   if (event.data.startsWith("BTN:")) {
    document.getElementById("status").innerHTML = event.data.substring(4);
  } else if (event.data.startsWith("DHT:")) {
    document.getElementById("dht").innerHTML = event.data.substring(4);
  } else if (event.data.startsWith("TOGGLE:")) {
    // รูปแบบข้อความ: TOGGLE:index:state
    // เช่น TOGGLE:1:ON หรือ TOGGLE:2:OFF
    let parts = event.data.split(":");
    let index = parseInt(parts[1]); // relay index 1-4
    let state = parts[2]; // ON หรือ OFF

    // index ใน HTML checkbox เริ่มต้นที่ 0 (array index)
    let checkbox = document.querySelectorAll(".toggle-group input")[index - 1];
    if (checkbox) {
      checkbox.checked = (state === "ON");
    }
  }
};
socket.onerror = function(error) {
  console.error("WebSocket error:", error);
};
socket.onclose = function() {
  console.warn("WebSocket closed");
};

function toggleRelay(index, checkbox) {
  let state = checkbox.checked ? "ON" : "OFF";
  socket.send("TOGGLE:" + index + ":" + state);
}
</script>
</head>
<body>
  <h1>Button State:</h1>
  <div id="status"><button class='btn gray'>Loading...</button></div>
  <h1>DHT Sensor:</h1>
  <div id="dht">LOADING...</div>
  <br><a href="/">Back</a>
  
    <h1>Relay Control:</h1>
  <div class="toggle-group">
    <label class="switch">
      <input type="checkbox" onchange="toggleRelay(1, this)">
      <span class="slider"></span>
    </label>
    <label class="switch">
      <input type="checkbox" onchange="toggleRelay(2, this)">
      <span class="slider"></span>
    </label>
    <label class="switch">
      <input type="checkbox" onchange="toggleRelay(3, this)">
      <span class="slider"></span>
    </label>
    <label class="switch">
      <input type="checkbox" onchange="toggleRelay(4, this)">
      <span class="slider"></span>
    </label>
  </div>
</body>
</html>
)rawliteral";

// ฟังก์ชันควบคุม LED
void ledGreen() {
  leds[0] = CRGB::Green;
  FastLED.show();
}

void ledBlack() {
  leds[0] = CRGB::Black;
  FastLED.show();
}

void handleControlsensor() {
  server.send(200, "text/html", HTML_PAGE);
}

void setupButton() {
  pinMode(buttonPin, INPUT_PULLUP);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(128);
  FastLED.clear();
  FastLED.show();
    // ตั้งค่า relay เป็น OUTPUT
  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // ปิดเริ่มต้น
    relayState[i] = false; // OFF
  }

  webSocket.begin();
  webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_CONNECTED) {
      String stateHTML = (buttonState == LOW)
        ? "<button class='btn green'>Pressed</button>"
        : "<button class='btn gray'>Released</button>";
      webSocket.sendTXT(num,"BTN:" + stateHTML);

      float h =  dht.readHumidity();
      float t = dht.readTemperature();
      if (!isnan(h) && !isnan(t)) {
        String dhtData = "Temp: " + String(t, 1) + "°C, Humidity: " + String(h, 1) + "%";
        webSocket.sendTXT(num, "DHT:" + dhtData);
      }
      // ส่งสถานะ relay ปัจจุบันให้ client ตอน connect
      for (int i=0; i<4; i++) {
        String state = relayState[i] ? "ON" : "OFF";
        webSocket.sendTXT(num, "TOGGLE:" + String(i+1) + ":" + state);
      }
    }
    else if (type == WStype_TEXT) {
      String msg = String((char*)payload);
      Serial.println("WS Message: " + msg);

      if (msg.startsWith("TOGGLE:")) {
        int idx = msg.substring(7, msg.indexOf(":", 7)).toInt();
        String state = msg.substring(msg.lastIndexOf(":") + 1);

        if (idx >= 1 && idx <= 4) {
          int pin = relayPins[idx - 1];
          if (state == "ON") {
            digitalWrite(pin, LOW);
            relayState[idx - 1] = true;
            if(idx==1){
              ledGreen();
            }
          } else {
            digitalWrite(pin, HIGH);
            relayState[idx - 1] = false;
            if(idx==1){
              ledBlack();
            }
          }
          Serial.printf("Relay %d => %s\n", idx, state.c_str());
        }
      }
    }
  });
}

void controlButton() {
  webSocket.loop();
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      String stateStr;
      if (buttonState == LOW) {
        stateStr = "<button class='btn green'>Pressed</button>";
        ledGreen();
           // เปิดรีเลย์ 1 (relayPins[0])
        relayState[0] = true;
        digitalWrite(relayPins[0], LOW); // เปิด
        Serial.println("Relay 1 turned ON");
        webSocket.broadcastTXT("TOGGLE:1:ON");
      } else {
        stateStr = "<button class='btn gray'>Released</button>";
        ledBlack();
            // ปิดรีเลย์ 1
        relayState[0] = false;
        digitalWrite(relayPins[0], HIGH); // ปิด
        Serial.println("Relay 1 turned OFF");
        webSocket.broadcastTXT("TOGGLE:1:OFF");
      }

      Serial.println("Button: " + stateStr);
      webSocket.broadcastTXT("BTN:" + stateStr);
    }
  }

  lastButtonState = reading;

  // อ่านและส่งข้อมูล DHT ทุก 2 วินาที
  if (millis() - lastDHTSend > DHTInterval) {
    lastDHTSend = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
      String dhtData = "Temp: " + String(t, 1) + "°C, Humidity: " + String(h, 1) + "%";
      Serial.println("DHT11: " + dhtData);
      webSocket.broadcastTXT("DHT:" + dhtData);
    }
  }
}

#endif