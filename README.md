# Street Fault Monitoring (ESP32)

A simple ESP32-based street light monitoring system that:
- reads street light status via an IR sensor
- measures current on two lamps using ACS712 sensors
- detects lamp faults using LDRs
- displays runtime status on a 16x2 LCD
- sends SMS alerts for faults via AT-command capable GSM modem
- publishes metrics to ThingSpeak

## Components

- ESP32 board (e.g., NodeMCU-32S)
- LiquidCrystal 16x2 display
- 2 x ACS712 current sensors
- 2 x LDR photoresistors
- 1 x IR motion/light sensor
- GSM module supporting AT commands (for SMS)
- Wi-Fi access point

## Pin mapping (in code)

- `IR_Sensor` -> 14
- `Street_Light_1` -> 12
- `Street_Light_2` -> 15
- `ACS1_Pin` -> 34
- `ACS2_Pin` -> 35
- `LDR1_Pin` -> 27
- `LDR2_Pin` -> 26
- LCD pins: `LiquidCrystal lcd(4,16,17,5,18,19)`

## Wi-Fi / ThingSpeak

- SSID: `Iot`
- Password: `iot12345`
- ThingSpeak channel ID: `3217881`
- ThingSpeak Write API Key: `AY033E197CMMIM7S`

> Update these values in `streetfault.ino` before upload.

## Behavior

- ESP32 connects to Wi-Fi and initializes Thingspeak
- Calibrates ACS712 offsets at startup
- If IR sensor is high, turns both street lights on else off
- Reads currents, shows L1/L2 current on LCD
- Detects faults if LDR says lamp should be lit but is not
- Sends one SMS per fault event
- Uploads data every 10s to ThingSpeak:
  - field1: current1
  - field2: current2
  - field3: fault1Count
  - field4: fault2Count
  - field5: status1 (lamp1 on)
  - field6: status2 (lamp2 on)

## Setup

1. Install Arduino IDE with ESP32 board support
2. Install libraries:
   - `LiquidCrystal`
   - `WiFi`
   - `ThingSpeak`
3. Wire the components as per pin mapping
4. Update Wi-Fi and ThingSpeak constants in `streetfault.ino`
5. Upload to ESP32

## Notes

- SMS number is hard-coded as `7200462481` in `sendSMS()`; change per your target
- This is a learning/demo prototype. For production, add more robust retry and security handling.

## GitHub Repository Checklist (Recommended)

- Add `.gitignore` for Arduino files and secrets:
  - `*.bin`, `*.elf`, `*.hex`, `*.cache`, `*.log`, `*.sync`, `*.DS_Store`
  - `*.zip`
- Include `LICENSE` (e.g., MIT) to clarify reuse rights
- Add this `README.md` as project docs
- Add `CHANGELOG.md` for version history
- Add `CONTRIBUTING.md` if others may contribute
- Add `docs/` folder for wiring diagrams and test results

## Local Development & Push Steps

1. Initialize git (if not already):
   ```bash
   git init
   git add .
   git commit -m "Initial commit: street fault monitoring" 
   ```
2. Create repo on GitHub and add remote:
   ```bash
   git remote add origin https://github.com/<your-username>/<repo-name>.git
   ```
3. Push branch:
   ```bash
   git branch -M main
   git push -u origin main
   ```

## Next Improvements

- Put sensitive credentials in a separate `config.h` not tracked by git
- Add schema for user-configurable settings (Wi-Fi, ThingSpeak, SMS number)
- Add a `serial` menu for runtime calibration and debugging
- Add error handling when Wi-Fi/ThingSpeak/SMS fails

