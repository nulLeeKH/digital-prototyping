#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DS3.h>

#include "preferences.h" 

float getAcc();
float getVol();
bool getBtn();

void getResponseServer();
void waitServer();

int status = WL_IDLE_STATUS;

const int deviceId = DEVICE_ID;
const int btnPin = BTN_PIN;

char domain[] = SERVER_DOMAIN;
const uint16_t port = 80;

bool privBtn = false;


void setup() {
  Serial.begin(9600);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to WiFi");
    status = WiFi.begin(WIFI_SSID, WIFI_PASS);

    delay(9000);
  }
  Serial.println("Connected to WiFi");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("signal strength (RSSI):");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  while (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    delay(3000);
  }

  pinMode(btnPin, INPUT);
}

void loop() {
  WiFiClient client;
    
  Serial.println("Send data to server...");
  if (client.connect(domain, port)) {
    Serial.println("Connected to server!");
    
    client.print("GET /api/upload/status/");
    client.print("0");
    client.print("?a=");
    client.print(getAcc());
    client.print("&v=");
    client.print(getVol());
    client.print("&b=");
    client.print(getBtn() ? 1 : 0);
    client.println(" HTTP/1.1");
    
    client.println("Host: dpt.nulleekh.com");
    
    client.println("Connection: close");
    
    client.println();
    
    Serial.println("Sent!");
  }

  Serial.println();
  waitServer(client);
  getResponseServer(client);
  Serial.println();
  
  Serial.println("Disconnecting from server.");
  
  client.stop();
}

float getAcc() {
  float x, y, z;

  while(!IMU.accelerationAvailable())
    continue;
  IMU.readAcceleration(x, y, z);
  
  return sqrt(sqrt(x*x+y*y)+z*z);
}

float getVol() {
  return float(map(analogRead(VOL_PIN), 0, 1024, 0, 100))/100.0;
}

bool getBtn() {
  bool currBtn, result;
  currBtn = digitalRead(btnPin);
  result = (privBtn != currBtn && currBtn);
  privBtn = currBtn;
  return result;
}

void getResponseServer(WiFiClient client) {
  Serial.println(F("\nUbidots' Server response:\n"));
  while (client.available()) {
    char c = client.read();
    Serial.print(c); // Uncomment this line to visualize the response on the Serial Monitor
  }
}

void waitServer(WiFiClient client) {
  int timeout = 0;
  while (!client.available() && timeout < 5000) {
    timeout++;
    delay(1);
    if (timeout >= 5000) {
      Serial.println(F("Error, max timeout reached"));
      break;
    }
  }
}
