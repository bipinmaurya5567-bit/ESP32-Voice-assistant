# ESP32 Voice Assistant

A voice-controlled AI assistant built on an ESP32. You speak, it answers
out loud through a speaker, looks things up on the web, and performs actions
such as turning a light on or off.

**Status: prototype (v0.1).** The hardware is built and tested in the Mokxi
simulator. The AI, web lookup and voice commands are the next steps.

## Hardware
ESP32 DevKit, INMP441 I2S microphone, MAX98357A I2S amplifier, 8 Ω speaker,
SSD1306 OLED, LED + resistor, breadboard.

## Status
| Part | Status |
|---|---|
| OLED SSD1306 | Working |
| LED | Working |
| Microphone INMP441 | Working |
| Speaker MAX98357A | Untested |
| Wi-Fi / AI | Not started |

## Pin map (ESP32)
- OLED: SDA D4, SCL D15
- LED: D18 (through resistor)
- Mic: SCK D26, WS D25, SD D32
- Speaker amp: BCLK D14, LRC D27, DIN D33

## Planned features
- [ ] Talk to the assistant and hear spoken replies
- [ ] Answer questions using information from the web
- [ ] Voice commands: "turn off the light"
- [ ] More devices (relay for a real lamp, sensors)

## Run
Open `diagram.mokxi.json` in Mokxi, or flash
`firmware/esp32_voice_assistant/esp32_voice_assistant.ino` to a real ESP32.
Libraries needed: Adafruit GFX, Adafruit SSD1306.
