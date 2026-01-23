#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>

WiFiManager wifiManager;

void setup() {
  Serial.begin(115200);
  Serial.println("\\nStarting WiFi Configuration Portal");
  
  // Set custom AP name and password
  wifiManager.setAPStaticIPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  
  // Automatically connect using saved credentials
  // If connection fails, it starts an access point with the specified name
  // You can visit 192.168.4.1 to configure WiFi
  if (!wifiManager.autoConnect("CharlesSmartHome_Wifi", "CharlesSmartHome2026")) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
  }
  
  // If you get here, you have connected to the WiFi
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Start mDNS responder
  if (MDNS.begin("charleshomewifi")) {
    Serial.println("mDNS responder started");
    Serial.println("You can access this device at: http://charleshomewifi.local");
  } else {
    Serial.println("Error starting mDNS");
  }
}

void loop() {
  // Your main code here
}