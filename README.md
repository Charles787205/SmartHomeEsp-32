# 🏠 SmartHome ESP32

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue?style=for-the-badge&logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Embedded-orange?style=for-the-badge&logo=platformio)
![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=cplusplus)

A modern, elegant IoT solution for controlling home appliances via WiFi using ESP32

[Features](#-features) • [API Reference](#-api-reference) • [Getting Started](#-getting-started) • [Hardware](#-hardware-requirements)

</div>

---

## ✨ Features

- 🌐 **WiFi Configuration Portal** - Easy setup with WiFiManager
- 🔌 **Multi-Switch Support** - Control unlimited GPIO pins
- 💾 **State Persistence** - Remembers switch states across reboots
- 🏷️ **Custom Names** - Assign meaningful names to each switch
- 🔍 **mDNS Discovery** - Auto-discovery on local network
- 🎯 **RESTful API** - Simple HTTP endpoints for control
- 📱 **Device Management** - Add, remove, and configure switches dynamically
- 🔄 **Remote Restart** - Reboot device via API

## 🛠️ Hardware Requirements

- **ESP32 Development Board**
- **Relay Module(s)** (for AC appliances)
- **Power Supply** (5V recommended)
- **Appliances** to control (lights, fans, etc.)

### Recommended GPIO Pins
- GPIO 17, 18, 19 (or any available GPIO pins)
- Avoid using GPIO 0, 2 (used for boot mode)

## 📡 API Reference

### Base URL
```
http://<device-ip>
# or
http://smart-home-<chip-id>.local
```

### Endpoints

#### 📋 Get All Switches
```http
GET /getSwitches
```
**Response:**
```json
[
  {
    "pin": 19,
    "name": "Electric Fan",
    "state": true
  },
  {
    "pin": 18,
    "name": "Living Room Light",
    "state": false
  }
]
```

#### ➕ Add Switch
```http
POST /addSwitch
Content-Type: application/x-www-form-urlencoded

pin=17&name=Bedroom Light
```
**Response:** `Switch added successfully. Please restart the device.`

#### 🗑️ Delete Switch
```http
POST /deleteSwitch
Content-Type: application/x-www-form-urlencoded

pin=17
```

#### 🔄 Toggle Switch
```http
POST /toggleSwitch
Content-Type: application/x-www-form-urlencoded

pin=19
```
**Response:**
```json
{
  "pin": 19,
  "state": "on"
}
```

#### ✏️ Update Switch Name
```http
POST /updateSwitchName
Content-Type: application/x-www-form-urlencoded

pin=19&name=New Name
```

#### 🏷️ Update Device Name
```http
POST /updateDeviceName
Content-Type: application/x-www-form-urlencoded

name=Living Room Controller
```

#### 🔄 Restart Device
```http
POST /restart
```

## 🚀 Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- [Visual Studio Code](https://code.visualstudio.com/) (recommended)
- USB cable for ESP32 programming

### Installation

1. **Clone the repository**
   ```bash
   git clone https://github.com/Charles787205/SmartHomeEsp-32.git
   cd SmartHomeEsp-32
   ```

2. **Open in PlatformIO**
   ```bash
   code .
   ```

3. **Build and Upload**
   ```bash
   pio run --target upload
   ```

4. **Monitor Serial Output**
   ```bash
   pio device monitor --baud 115200
   ```

### First-Time Setup

1. **Connect to WiFi Portal**
   - On first boot, ESP32 creates an access point
   - SSID: `CharlesSmartHome_Wifi`
   - Password: `CharlesSmartHome2026`
   - Navigate to `192.168.4.1`

2. **Configure WiFi**
   - Select your WiFi network
   - Enter password
   - Device will connect and obtain IP address

3. **Access Device**
   - Check Serial Monitor for IP address
   - Or use mDNS: `http://smart-home-<chip-id>.local`

## 💡 Usage Examples

### cURL Examples

**Add a new switch:**
```bash
curl -X POST http://192.168.1.100/addSwitch \
  -d "pin=19&name=Ceiling Fan"
```

**Toggle switch:**
```bash
curl -X POST http://192.168.1.100/toggleSwitch \
  -d "pin=19"
```

**Get all switches:**
```bash
curl http://192.168.1.100/getSwitches
```

### Python Example

```python
import requests

BASE_URL = "http://192.168.1.100"

# Add a switch
requests.post(f"{BASE_URL}/addSwitch", 
              data={"pin": 19, "name": "Living Room Light"})

# Toggle switch
response = requests.post(f"{BASE_URL}/toggleSwitch", 
                        data={"pin": 19})
print(response.json())  # {"pin": 19, "state": "on"}

# Get all switches
switches = requests.get(f"{BASE_URL}/getSwitches").json()
for switch in switches:
    print(f"{switch['name']}: {'ON' if switch['state'] else 'OFF'}")
```

### JavaScript/Node.js Example

```javascript
const axios = require('axios');

const BASE_URL = 'http://192.168.1.100';

// Toggle switch
async function toggleSwitch(pin) {
  const response = await axios.post(`${BASE_URL}/toggleSwitch`, 
    new URLSearchParams({ pin: pin.toString() })
  );
  console.log(response.data);
}

// Get all switches
async function getSwitches() {
  const response = await axios.get(`${BASE_URL}/getSwitches`);
  return response.data;
}

toggleSwitch(19);
```

## 🔧 Configuration

### WiFi Settings

Modify in `main.cpp`:
```cpp
wifiManager.autoConnect("YourAPName", "YourPassword");
```

### Pin Configuration

Configure pins dynamically via API or modify defaults in code:
```cpp
// Pins are stored in Preferences and loaded on boot
// No hardcoding required!
```

## 📂 Project Structure

```
SmartHome-Esp32/
├── src/
│   └── main.cpp          # Main application code
├── include/              # Header files
├── lib/                  # Custom libraries
├── test/                 # Unit tests
├── platformio.ini        # PlatformIO configuration
└── README.md            # This file
```

## 🔐 Security Considerations

⚠️ **Important Notes:**
- Change default WiFi portal credentials
- Use on trusted networks only
- Consider implementing authentication for production use
- HTTPS is not implemented - use VPN for remote access

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is open source and available under the [MIT License](LICENSE).

## 👤 Author

**Charles**

- GitHub: [@Charles787205](https://github.com/Charles787205)
- Repository: [SmartHomeEsp-32](https://github.com/Charles787205/SmartHomeEsp-32)

## 🙏 Acknowledgments

- ESP32 Arduino Core
- WiFiManager by tzapu
- ArduinoJson by Benoit Blanchon
- PlatformIO

---

<div align="center">

Made with ❤️ using ESP32

⭐ Star this repo if you find it helpful!

</div>
