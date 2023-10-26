#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer server(80);

const int ledPin = D6;
DHT dht14(D4, DHT11);

const char* ssid = "pinsudaa";
const char* password = "123456zx";

void setup() {
  Serial.begin(115200);
  initWiFi(ssid, password);

  pinMode(ledPin, OUTPUT);
  dht14.begin();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/TurnOn", HTTP_GET, handleTurnOn);
  server.on("/TurnOff", HTTP_GET, handleTurnOff);
  server.on("/data", HTTP_GET, handleData);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void initWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  float temperature = dht14.readTemperature();
  float humidity = dht14.readHumidity();
  String html = "<html><head><style>body{font-family:Arial,Helvetica,sans-serif;background-color:#F1B4BB;}";
  html += "div{background-color:#E0E0E0;border-radius:10px;padding:20px;margin:20px;text-align:center;}";
  html += "h1{color:#333;font-size:36px;text-decoration:underline;}p{color:#444;font-size:24px;font-weight:bold;}";
  html += "input[type='submit']{background-color:#4CAF50;color:white;padding:10px 20px;border:none;";
  html += "border-radius:5px;cursor:pointer;font-size:16px;width:150px;margin:10px auto;}";
  html += "input[type='submit']:hover{background-color:#45a049;}#turnOffButton{background-color:#FF0000;}</style></head><body>";
  html += "<div><h1>ESP8266 Web Server</h1><p>Temperature: <span id='temperature'>" + String(temperature, 2) + " C</span></p>";
  html += "<p>Humidity: <span id='humidity'>" + String(humidity, 2) + " %</span></p><p>LED Status: ";
  html += digitalRead(ledPin) == HIGH ? "<span style='color:green;'>On</span>" : "<span style='color:red;'>Off</span>";
  html += "</p><form action='/TurnOn' method='get'><input type='submit' value='Turn On'></form>";
  html += "<form action='/TurnOff' method='get'><input type='submit' id='turnOffButton' value='Turn Off'></form></div></body></html>";
  html += "<script>function updateData(){var xhr=new XMLHttpRequest();xhr.open('GET','/data',true);xhr.onreadystatechange=function(){";
  html += "if(xhr.readyState==4&&xhr.status==200){var data=JSON.parse(xhr.responseText);";
  html += "document.getElementById('temperature').textContent=data.temperature.toFixed(2)+' C';";
  html += "document.getElementById('humidity').textContent=data.humidity.toFixed(2)+' %';}};xhr.send();}setInterval(updateData,1000);</script>";
  server.send(200, "text/html", html);
}


void handleTurnOn() {
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleTurnOff() {
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleData() {
  float temperature = dht14.readTemperature();
  float humidity = dht14.readHumidity();
  String data = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
  server.send(200, "application/json", data);
}
