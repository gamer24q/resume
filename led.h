  #ifndef LED_H
  #define LED_H

  #include <WebServer.h>
  #include <FastLED.h>
  #include "sidebar.h"  // นำ sidebar เข้ามาใช้

  extern WebServer server; // ประกาศไว้ใน sketch หรือ homeweb.h

  #define LED_PIN     48
  #define NUM_LEDS    1
  CRGB leds[NUM_LEDS];

  uint8_t brightness = 128; 

  enum Mode {STATIC, BLINK, RAINBOW};
  Mode currentMode = STATIC;

  uint8_t staticR = 255, staticG = 0, staticB = 0;
  unsigned long lastBlinkTime = 0;
  bool blinkState = false;
  uint8_t rainbowHue = 0;

const char* htmlPage = R"rawliteral(
  <div class="card">
    <h2>ควบคุมไฟ LED</h2>

    <label>เลือกโหมด:</label>
    <select id="mode">
      <option value="static">เปิดสีเดียว</option>
      <option value="blink">ไฟกระพริบ</option>
      <option value="rainbow">ไล่สี</option>
    </select>
    <br>

    <label>เลือกสี:</label>
    <input type="color" id="colorPicker" value="#ff0000" oninput="updatePreview()">
    <div id="colorPreview"></div>
    <br>

    <label>ความสว่าง (0-255):</label>
    <input type="number" id="brightness" min="0" max="255" value="128">
    <br>

    <button onclick="sendColor()">ตกลง</button>
  </div>

  <style>
    body {
      margin: 0;
      padding: 0;
      font-family: 'Segoe UI', sans-serif;
      background: linear-gradient(to right, #2c3e50, #4ca1af);
      height: 100vh;
      color: #fff;
    }
    .card {
      background: rgba(255, 255, 255, 0.1);
      padding: 30px 40px;
      border-radius: 16px;
      backdrop-filter: blur(10px);
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.3);
      text-align: center;
      max-width: 400px;
      width: 90%;
      margin: auto;
      margin-top: 100px;
    }
    h2 {
      margin-bottom: 20px;
    }
    label {
      display: block;
      margin: 15px 0 5px;
      font-weight: 600;
      font-size: 1.1em;
    }
    select, input[type="color"], input[type="number"] {
      padding: 8px 10px;
      border-radius: 8px;
      border: none;
      font-size: 1em;
      width: 100%;
      max-width: 200px;
      cursor: pointer;
      color: #333;
    }
    #colorPreview {
      width: 50px;
      height: 50px;
      display: inline-block;
      vertical-align: middle;
      border: 1px solid #ccc;
      margin-left: 10px;
      border-radius: 8px;
      background-color: #ff0000;
    }
    input[type="number"] {
      -moz-appearance: textfield;
    }
    input[type="number"]::-webkit-inner-spin-button,
    input[type="number"]::-webkit-outer-spin-button {
      -webkit-appearance: none;
      margin: 0;
    }
    button {
      margin-top: 25px;
      background-color: #00c3ff;
      border: none;
      padding: 12px 25px;
      color: white;
      border-radius: 8px;
      font-size: 1.1em;
      cursor: pointer;
      transition: background 0.3s ease;
      width: 100%;
      max-width: 220px;
    }
    button:hover {
      background-color: #009fcc;
    }
  </style>

  <script>
    function updatePreview() {
      const color = document.getElementById("colorPicker").value;
      document.getElementById("colorPreview").style.backgroundColor = color;
    }

    function sendColor() {
      const mode = document.getElementById("mode").value;
      const color = document.getElementById("colorPicker").value.substring(1);
      const brightness = document.getElementById("brightness").value;
      fetch("/setColor?mode=" + mode + "&color=" + color + "&brightness=" + brightness);
    }

    window.onload = updatePreview;
  </script>
)rawliteral";


  // --- Web Handlers ---
  void handleLedRoot() {
    server.sendHeader("Content-Type", "text/html; charset=utf-8");

    String html = FPSTR(SIDEBAR_HTML);  // ✅ เริ่มจาก sidebar
    html += htmlPage;                   // ✅ เนื้อหา HTML หลัก
    html += FPSTR(SIDEBAR_END);         // ✅ ปิดเนื้อหา

    server.send(200, "text/html", html);
  }


  void handleSetColor() {
    String mode = server.arg("mode");
    String color = server.arg("color");
    brightness = server.arg("brightness").toInt();

    long number = strtol(color.c_str(), NULL, 16);
    staticR = (number >> 16) & 0xFF;
    staticG = (number >> 8) & 0xFF;
    staticB = number & 0xFF;

    FastLED.setBrightness(brightness);

    if (mode == "static") {
      currentMode = STATIC;
      leds[0] = CRGB(staticR, staticG, staticB);
      FastLED.show();
    } else if (mode == "blink") {
      currentMode = BLINK;
    } else if (mode == "rainbow") {
      currentMode = RAINBOW;
    }

    server.sendHeader("Content-Type", "text/plain; charset=utf-8");
    server.send(200, "text/plain", "OK");
  }

  // --- เรียกจาก setup() ---
  void setupWebLed() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.clear();
    FastLED.setBrightness(brightness);
    FastLED.show();

    server.on("/led", handleLedRoot);
    server.on("/setColor", handleSetColor);
  }

  // --- เรียกจาก loop() ---
  void updateLedEffect() {
    if (currentMode == BLINK) {
      if (millis() - lastBlinkTime > 300) {
        blinkState = !blinkState;
        leds[0] = blinkState ? CRGB(staticR, staticG, staticB) : CRGB::Black;
        FastLED.setBrightness(brightness);  
        FastLED.show();
        lastBlinkTime = millis();
      }
    } else if (currentMode == RAINBOW) {
      leds[0] = CHSV(rainbowHue++, 255, 255);
      FastLED.setBrightness(brightness);
      FastLED.show();
      delay(20);
    }
  }

  #endif
