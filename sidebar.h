#ifndef SIDEBAR_H
#define SIDEBAR_H

const char SIDEBAR_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>LED Control Panel</title>
  <style>
    body {
      margin: 0;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: #f4f4f4;
    }
    .sidebar {
      position: fixed;
      top: 0;
      left: 0;
      bottom: 0;
      width: 200px;
      background-color: #2c3e50;
      padding-top: 20px;
      color: white;
    }
    .sidebar h3 {
      text-align: center;
      margin-bottom: 20px;
      color: #ecf0f1;
    }
    .sidebar a {
      display: block;
      padding: 12px 20px;
      color: #ecf0f1;
      text-decoration: none;
      transition: background 0.3s;
    }
    .sidebar a:hover {
      background-color: #34495e;
    }
    .content {
      margin-left: 220px;
      padding: 20px;
    }
  </style>
</head>
<body>
  <div class="sidebar">
    <h3>Menu</h3>
    <a href="/">🏠 Home</a>
    <a href="/led">💡 LED Control</a>
    <a href="/sensor">📈 Sensors</a>
    <a href="/firmware">⚙️ Firmware</a>
  </div>
  <div class="content">
)rawliteral";

const char SIDEBAR_END[] PROGMEM = R"rawliteral(
  </div>
</body>
</html>
)rawliteral";

#endif

