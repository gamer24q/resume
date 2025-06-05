#ifndef HOMEWEB_H
#define HOMEWEB_H

#include <WebServer.h>
#include "led.h"
#include "sensor.h"
#include "Firmware.h"

WebServer server(80);

// หน้าแรก
void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Smart Home</title>
      <link rel="icon" type="image/png" href="/favicon.png">
      <style>
        body {
          margin: 0;
          padding: 0;
          font-family: 'Poppins', sans-serif;
          background: radial-gradient(circle at top left, #2c3e50, #1a252f);
          display: flex;
          justify-content: center;
          align-items: center;
          height: 100vh;
          color: #ffffff;
        }
        .glass-card {
          background: rgba(255, 255, 255, 0.05);
          border-radius: 20px;
          padding: 40px 30px;
          box-shadow: 8px 8px 20px rgba(0, 0, 0, 0.5),
                      -8px -8px 20px rgba(255, 255, 255, 0.03);
          backdrop-filter: blur(12px);
          text-align: center;
          width: 90%;
          max-width: 400px;
        }
        h1 {
          font-size: 2.5em;
          margin-bottom: 30px;
          color: #00d9ff;
          text-shadow: 0 2px 4px rgba(0,0,0,0.4);
        }
        .button {
          display: block;
          margin: 15px 0;
          padding: 15px;
          background: linear-gradient(145deg, #3a4a5a, #1b232a);
          border: none;
          border-radius: 12px;
          color: white;
          font-size: 1.2em;
          text-decoration: none;
          box-shadow: inset 1px 1px 5px rgba(255,255,255,0.1),
                      4px 4px 10px rgba(0,0,0,0.4);
          transition: transform 0.2s, background 0.3s;
        }
        .button:hover {
          background: linear-gradient(145deg, #00c3ff, #008fb3);
          transform: scale(1.03);
        }
        @media (max-width: 480px) {
          h1 { font-size: 2em; }
          .button { font-size: 1em; padding: 12px; }
        }
      </style>
    </head>
    <body>
      <div class="glass-card">
        <h1>🏠 SMART HOME</h1>
        <a class="button" href="/led">💡 LED Control</a>
        <a class="button" href="/sensor">🌡️ Sensor Monitor</a>
        <a class="button" href="/firmware">🛠️ Update Firmware</a>
      </div>
    </body>
    </html>
  )rawliteral");
}



// ตั้งค่า routes หลัก (ส่วนนี้ไม่ต้องยุ่งกับ LED/Sensor โดยตรง)
void setupWebServer() {
  server.on("/", handleRoot);
  server.on("/led", handleLedRoot);
  server.on("/sensor", handleControlsensor);
  server.on("/firmware", handleFirmwareUpdate);
  server.begin();
  setupButton();
}
  // Route พื้นฐานจะไปลงที่ led.h/sensor.h เอง


// loop สำหรับ server
void handleWebServer() {
  server.handleClient();
  controlButton();
}

#endif
