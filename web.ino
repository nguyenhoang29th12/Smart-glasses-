#include <Wire.h>
#include <VL53L0X.h>
#include <WiFi.h>
#include <WebServer.h>

VL53L0X sensor;


#define SDA_PIN 8
#define SCL_PIN 9
#define VIBRATION_MOTOR_PIN 3

// Thông tin mạng Wi-Fi riêng (Access Point)
const char* apSSID = "Vibration_Control";   
const char* apPassword = "12345678";        

IPAddress local_IP(192, 168, 4, 1);      
IPAddress gateway(192, 168, 4, 1);        
IPAddress subnet(255, 255, 255, 0);       
WebServer server(80);
int distanceThreshold = 200;
bool motorState = false;

// Khai báo hàm trước khi gọi
void handleSetThreshold();
void handleSetMotor();

void setup() {
  Serial.begin(115200);

 
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Mạng Wi-Fi riêng đã được tạo!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());  
  
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!sensor.init()) {
    Serial.println("Không thể khởi tạo cảm biến VL53L0X!");
    while (1);
  }
  sensor.setTimeout(500);
  sensor.startContinuous();

  pinMode(VIBRATION_MOTOR_PIN, OUTPUT);
  digitalWrite(VIBRATION_MOTOR_PIN, LOW);


  server.on("/", HTTP_GET, handleRoot);
  server.on("/setThreshold", HTTP_GET, handleSetThreshold);
  server.on("/setMotor", HTTP_GET, handleSetMotor);
  server.begin();
  Serial.println("Web server đã khởi chạy!");
}

void loop() {
  server.handleClient(); 

  int distance = sensor.readRangeContinuousMillimeters();
  if (sensor.timeoutOccurred()) {
    Serial.println("Timeout!");
    return;
  }

 
  if (distance > 0 && distance <= distanceThreshold) {
    digitalWrite(VIBRATION_MOTOR_PIN, HIGH);  
    motorState = true;
  } else {
    digitalWrite(VIBRATION_MOTOR_PIN, LOW);   
    motorState = false;
  }
}


void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Điều Khiển Động Cơ Rung</title>
  <style>
    /* RESET */
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    /* BODY */
    body {
      font-family: 'Arial', sans-serif;
      background: linear-gradient(to right, #222E50, #A1B5D8);
      color: #333;
      display: flex;
      justify-content: center;
      align-items: center;
      flex-direction: column;
      min-height: 100vh;
      padding: 20px;
      animation: backgroundSlide 5s infinite alternate ease-in-out;
    }

    /* HEADER */
    h1 {
      background: #222E50;
      color: #A1B5D8;
      padding: 15px;
      text-align: center;
      margin: 0;
      border-radius: 8px 8px 0 0;
      font-size: 24px;
      letter-spacing: 2px;
      text-transform: uppercase;
      box-shadow: 0px 5px 15px rgba(0, 0, 0, 0.2);
      animation: fadeInDown 1.2s ease-in-out;
    }

    /* CONTAINER */
    .container {
      max-width: 600px;
      width: 100%;
      background: #ffffff;
      border-radius: 8px;
      box-shadow: 0px 5px 15px rgba(0, 0, 0, 0.2);
      overflow: hidden;
      animation: fadeInUp 1.2s ease-in-out;
    }

    .container p {
      font-size: 18px;
      padding: 10px;
      text-align: center;
      font-weight: bold;
    }

    .status {
      font-weight: bold;
      color: #4CAF50;
      display: inline-block;
      animation: pulse 1.5s infinite;
    }

    /* FORM */
    form {
      padding: 15px 20px;
    }

    label {
      display: block;
      margin-bottom: 10px;
      font-weight: bold;
      font-size: 16px;
    }

    input,
    select,
    button {
      width: 100%;
      padding: 12px;
      margin-bottom: 15px;
      border: 1px solid #ddd;
      border-radius: 5px;
      font-size: 16px;
      transition: all 0.3s ease;
    }

    input:focus,
    select:focus {
      border-color: #222E50;
      outline: none;
      box-shadow: 0 0 5px rgba(34, 46, 80, 0.5);
    }

    /* BUTTON */
    button {
      background-color: #222E50;
      color: #ffffff;
      border: none;
      cursor: pointer;
      font-weight: bold;
      text-transform: uppercase;
      letter-spacing: 1px;
      transition: background 0.3s ease, transform 0.2s ease;
    }

    button:hover {
      background-color: #A1B5D8;
      transform: scale(1.05);
    }

    button:active {
      transform: scale(1);
    }

    /* FOOTER */
    .footer {
      margin-top: 20px;
      text-align: center;
      color: #fff;
      font-size: 14px;
      animation: fadeIn 2s ease-in-out;
    }

    .footer span {
      font-weight: bold;
      animation: textGlow 1.5s infinite alternate;
    }

    /* MEDIA QUERY (RESPONSIVE) */
    @media screen and (max-width: 768px) {
      body {
        padding: 10px;
      }

      h1 {
        font-size: 20px;
      }

      p {
        font-size: 16px;
      }

      input,
      select,
      button {
        font-size: 14px;
      }
    }

    /* ANIMATIONS */
    @keyframes fadeInUp {
      from {
        opacity: 0;
        transform: translateY(30px);
      }
      to {
        opacity: 1;
        transform: translateY(0);
      }
    }

    @keyframes fadeInDown {
      from {
        opacity: 0;
        transform: translateY(-30px);
      }
      to {
        opacity: 1;
        transform: translateY(0);
      }
    }

    @keyframes backgroundSlide {
      from {
        background: linear-gradient(to right, #222E50, #A1B5D8);
      }
      to {
        background: linear-gradient(to left, #222E50, #6B9AC4);
      }
    }

    @keyframes textGlow {
      from {
        text-shadow: 0 0 10px #fff, 0 0 20px #4CAF50, 0 0 30px #81C784;
      }
      to {
        text-shadow: 0 0 5px #fff, 0 0 15px #81C784, 0 0 25px #4CAF50;
      }
    }

    @keyframes pulse {
      0%, 100% {
        color: #4CAF50;
      }
      50% {
        color: #81C784;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Điều Khiển Khoảng Cách</h1>
    <div class="content">
        <p>Khoảng cách ngưỡng hiện tại: <span class="status">)rawliteral" + String(distanceThreshold) + R"rawliteral( mm</span></p>
      <form action="/setThreshold">
        <label for="threshold">Nhập khoảng cách ngưỡng (mm):</label>
        <input type="number" id="threshold" name="threshold" value=")rawliteral" + String(distanceThreshold) + R"rawliteral(" required>
        <button type="submit">Cập nhật</button>
      </form>
      <p>Trạng thái động cơ: <span class="status">)rawliteral" + String(motorState ? "BẬT" : "TẮT") + R"rawliteral(</span></p>
      <form action="/setMotor">
        <label for="state">Bật/Tắt động cơ:</label>
        <select id="state" name="state">
          <option value="1">Bật</option>
          <option value="0">Tắt</option>
        </select>
        <button type="submit">Gửi</button>
      </form>
    </div>
  </div>
  <div class="footer">
    <p>Thiết kế bởi <span>Nguyễn Thế Hoàng</span> | Liên hệ: facebook <a href="https://facebook.com/bomaytenhoang" style="color: #81C784; text-decoration: none;">nguyenhoang121315@gmail.com</a></p>
  </div>
</body>
</html>

)rawliteral";

  server.send(200, "text/html", html);
}


void handleSetThreshold() {
  if (server.hasArg("threshold")) {
    distanceThreshold = server.arg("threshold").toInt();
    Serial.println("Cập nhật khoảng cách ngưỡng: " + String(distanceThreshold) + " mm");
  }
  server.sendHeader("Location", "/");
  server.send(303);
}


void handleSetMotor() {
  if (server.hasArg("state")) {
    bool newState = server.arg("state").toInt();
    digitalWrite(VIBRATION_MOTOR_PIN, newState ? HIGH : LOW);
    motorState = newState;
    Serial.println("Trạng thái động cơ: " + String(motorState ? "BẬT" : "TẮT"));
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
