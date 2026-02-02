#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>
#include <WebServer.h>  
#include <Preferences.h>
#include <ArduinoJson.h>
#include <vector>
using namespace std;
WiFiManager wifiManager;
WebServer server(80);

vector<int> switchPins;
vector<bool> switchStates;
vector<String> switchNames;
Preferences prefs;

String deviceName = "not assigned";

void getSavedStates() {
  prefs.begin("switches", true);
  deviceName = prefs.getString("deviceName", "not assigned");
  Serial.println("Device Name: " + deviceName);
  
  String pins = prefs.getString("switchPins", "");
  Serial.println("Loading pins from preferences: '" + pins + "'");
  
  //slice pins by comma
  int start = 0;
  int end = pins.indexOf(',');
  while (end != -1) {
    String pinStr = pins.substring(start, end);
    int pin = pinStr.toInt();
    Serial.println("Loaded pin: " + String(pin));
    if (pin != 0) {
      switchPins.push_back(pin);
    }
    start = end + 1;
    end = pins.indexOf(',', start);
  }
  
  // Don't forget the last pin after the final comma (or the only pin if no comma)
  if (pins.length() > 0) {
    String lastPinStr = pins.substring(start);
    int lastPin = lastPinStr.toInt();
    Serial.println("Loaded last pin: " + String(lastPin));
    if (lastPin != 0) {
      switchPins.push_back(lastPin);
    }
  }

  for (size_t i = 0; i < switchPins.size(); i++) {
    String stateKey = "switchState_" + String(switchPins[i]);
    bool state = prefs.getBool(stateKey.c_str(), false);
    String switchName = prefs.getString(("switchName_" + String(switchPins[i])).c_str(), "Switch " + String(i+1));
    switchNames.push_back(switchName);
    switchStates.push_back(state);
    Serial.println("Switch Pin: " + String(switchPins[i]) + " State: " + String(state ? "ON" : "OFF"));
  }
  
  
  prefs.end();
}

void handleAddSwitch() {
  if (server.method() == HTTP_POST) {
    String pinStr = server.arg("pin");
    String nameStr = server.arg("name");
    int pin = pinStr.toInt();
    Serial.println("=== ADD SWITCH DEBUG ===");
    Serial.println("Pin to add: " + String(pin));
    Serial.println("Name: " + nameStr);
    
    if (pin != 0 && nameStr.length() > 0) {
      // Check if pin already exists in runtime array
      bool pinExists = false;
      for (size_t i = 0; i < switchPins.size(); i++) {
        if (switchPins[i] == pin) {
          Serial.println("Pin already exists in runtime array at index " + String(i));
          pinExists = true;
          break;
        }
      }
      
      if (pinExists) {
        server.send(400, "text/plain", "Pin already exists.");
        return;
      }
      
      Serial.println("Pin does not exist, adding...");
      prefs.begin("switches", false);
      
      // Rebuild the pins string from the current runtime array plus the new pin
      String newPins = "";
      for (size_t i = 0; i < switchPins.size(); i++) {
        if (newPins.length() > 0) {
          newPins += ",";
        }
        newPins += String(switchPins[i]);
      }
      if (newPins.length() > 0) {
        newPins += ",";
      }
      newPins += String(pin);
      
      Serial.println("New pins string: '" + newPins + "'");
      prefs.putString("switchPins", newPins);
      // Save switch name
      prefs.putString(("switchName_" + String(pin)).c_str(), nameStr);
      // Save initial state as OFF
      prefs.putBool(("switchState_" + String(pin)).c_str(), false);
      prefs.end();
      
      server.send(200, "text/plain", "Switch added successfully. Please restart the device.");


    } else {
      server.send(400, "text/plain", "Invalid pin or name.");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleDeleteSwitch(){
  if (server.method() == HTTP_POST) {
    String pinStr = server.arg("pin");
    int pin = pinStr.toInt();
    if (pin != 0) {
      prefs.begin("switches", false);
      // Remove pin from saved pins
      String existingPins = prefs.getString("switchPins", "");
      String newPins = "";
      int start = 0;
      int end = existingPins.indexOf(',');
      while (end != -1) {
        String currentPinStr = existingPins.substring(start, end);
        if (currentPinStr.toInt() != pin) {
          if (newPins.length() > 0) {
            newPins += ",";
          }
          newPins += currentPinStr;
        }
        start = end + 1;
        end = existingPins.indexOf(',', start);
      }
      // Check last pin
      String lastPinStr = existingPins.substring(start);
      if (lastPinStr.toInt() != pin) {
        if (newPins.length() > 0) {
          newPins += ",";
        }
        newPins += lastPinStr;
      }
      prefs.putString("switchPins", newPins);
      // Remove switch name and state
      prefs.remove(("switchName_" + String(pin)).c_str());
      prefs.remove(("switchState_" + String(pin)).c_str());
      prefs.end();
      
      server.send(200, "text/plain", "Switch deleted successfully. Please restart the device.");
    } else {
      server.send(400, "text/plain", "Invalid pin.");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleToggleSwitch() {
  if (server.method() == HTTP_POST) {
    String pinStr = server.arg("pin");
    int pin = pinStr.toInt();
    for (size_t i = 0; i < switchPins.size(); i++) {
      if (switchPins[i] == pin) {
        // Toggle state
        switchStates[i] = !switchStates[i];
        digitalWrite(pin, switchStates[i] ? HIGH : LOW);
        
        // Save new state
        prefs.begin("switches", false);
        prefs.putBool(("switchState_" + String(pin)).c_str(), switchStates[i]);
        prefs.end();
        
        JsonDocument doc;
        doc["pin"] = pin;
        doc["state"] = switchStates[i] ? "on" : "off";
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
        return;
      }
    }
    server.send(404, "text/plain", "Switch not found.");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleUpdateSwitchName() {
  if (server.method() == HTTP_POST) {
    prefs.begin("switches", false);
    String pinStr = server.arg("pin");
    String nameStr = server.arg("name");
    int pin = pinStr.toInt();
    if (pin != 0 && nameStr.length() > 0) {
      prefs.begin("switches", false);
      prefs.putString(("switchName_" + String(pin)).c_str(), nameStr);
      prefs.end();
      
      server.send(200, "text/plain", "Switch name updated successfully.");
    } else {
      server.send(400, "text/plain", "Invalid pin or name.");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
  
}

void handleGetSwitches() {
  JsonDocument doc;
  JsonArray switches = doc.to<JsonArray>();
  for (size_t i = 0; i < switchPins.size(); i++) {
    JsonObject sw = switches.add<JsonObject>();
    sw["pin"] = switchPins[i];
    sw["name"] = switchNames[i];
    sw["state"] = (bool)switchStates[i];
  }
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}


void handleDeviceName(){
  if (server.method() == HTTP_POST) {
    String nameStr = server.arg("name");
    if (nameStr.length() > 0) {
      prefs.begin("switches", false);
      prefs.putString("deviceName", nameStr);
      prefs.end();
      
      server.send(200, "text/plain", "Device name updated successfully. Please restart the device.");
    } else {
      server.send(400, "text/plain", "Invalid name.");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
  
}

void handleRestart() {
  if (server.method() == HTTP_POST) {
    server.send(200, "text/plain", "Device restarting...");
    delay(500);
    ESP.restart();
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\\nStarting WiFi Configuration Portal");
  
  getSavedStates(); 
  for (int i = 0; i < switchPins.size(); i++) {
    pinMode(switchPins[i], OUTPUT);
    digitalWrite(switchPins[i], LOW);
  }
  for (int i = 0; i < switchPins.size(); i++) {
    if (switchStates[i]) {
      digitalWrite(switchPins[i], HIGH);
    }
  }
  
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
  

  uint64_t chipId = ESP.getEfuseMac(); // The chip ID is essentially its MAC address
  char hostName[32];
  sprintf(hostName, "smart-home-%llx", (uint16_t)(chipId & 0xFFFF));
  
  if (MDNS.begin(hostName)) {
    MDNS.addService("smarthome", "tcp", 80);
    MDNS.addServiceTxt("smarthome", "tcp", "device name", deviceName.c_str());
    
    Serial.println("mDNS responder started");
    Serial.println("You can access this device at: http://" + String(hostName) + ".local");
  } else {
    Serial.println("Error starting mDNS");
  }

  // Define server routes
  server.on("/addSwitch", handleAddSwitch);
  server.on("/deleteSwitch", handleDeleteSwitch);
  server.on("/toggleSwitch", handleToggleSwitch);
  server.on("/updateSwitchName", handleUpdateSwitchName);
  server.on("/getSwitches", handleGetSwitches);
  server.on("/updateDeviceName", handleDeviceName);
  server.on("/restart", handleRestart);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
}

