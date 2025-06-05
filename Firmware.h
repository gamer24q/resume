#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <WebServer.h>
#include <Update.h>
#include "sidebar.h"

extern WebServer server;

// เอาโค้ด HTML ของหน้า Firmware UI มาเก็บเป็นตัวแปร string
const char firmwarePage[] PROGMEM = R"rawliteral(
  <div class="card">
    <h2>OTA Firmware Update</h2>
    <form method="POST" action="/firmware" enctype="multipart/form-data">
      <input type="file" name="update" required><br>
      <input type="submit" value="Upload & Update">
    </form>
    <a href="/">← Back to Home</a>
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
    input[type="file"] {
      margin-bottom: 20px;
      color: white;
    }
    input[type="submit"] {
      background-color: #00c3ff;
      border: none;
      padding: 12px 25px;
      color: white;
      border-radius: 8px;
      font-size: 1em;
      cursor: pointer;
      transition: background 0.3s ease;
    }
    input[type="submit"]:hover {
      background-color: #009fcc;
    }
    a {
      display: inline-block;
      margin-top: 20px;
      text-decoration: none;
      color: #ffffff;
      font-weight: bold;
    }
  </style>
)rawliteral";

void handleFirmwareUpdate() {
  if (server.method() == HTTP_POST) {
    HTTPUpload& upload = server.upload();

    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update Start: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u bytes\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }

      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", "Update complete. Rebooting...");
      delay(1000);
      ESP.restart();
    }
  } else {
    // ส่งหน้า HTML โดยมี sidebar ครอบเนื้อหา firmwarePage
    String html = FPSTR(SIDEBAR_HTML);
    html += FPSTR(firmwarePage);
    html += FPSTR(SIDEBAR_END);
    server.send(200, "text/html", html);
  }
}

#endif
