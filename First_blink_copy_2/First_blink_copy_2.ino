#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/i2s.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

const int kLed = 18;
const int kThreshold = 600;  // mic level that triggers the beep

unsigned long lastBlink = 0, lastDraw = 0, lastBeep = 0;
bool ledOn = false;

void setupI2S(i2s_port_t port, bool rx, int bck, int ws, int dataPin) {
  i2s_config_t c;
  memset(&c, 0, sizeof(c));
  c.mode = (i2s_mode_t)(I2S_MODE_MASTER | (rx ? I2S_MODE_RX : I2S_MODE_TX));
  c.sample_rate = 16000;
  c.bits_per_sample = rx ? I2S_BITS_PER_SAMPLE_32BIT : I2S_BITS_PER_SAMPLE_16BIT;
  c.channel_format = rx ? I2S_CHANNEL_FMT_ONLY_LEFT : I2S_CHANNEL_FMT_RIGHT_LEFT;
  c.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  c.dma_buf_count = 4;
  c.dma_buf_len = 256;
  i2s_pin_config_t p;
  memset(&p, 0, sizeof(p));
  p.bck_io_num = bck;
  p.ws_io_num = ws;
  p.data_out_num = rx ? I2S_PIN_NO_CHANGE : dataPin;
  p.data_in_num = rx ? dataPin : I2S_PIN_NO_CHANGE;
  p.mck_io_num = I2S_PIN_NO_CHANGE;
  i2s_driver_install(port, &c, 0, NULL);
  i2s_set_pin(port, &p);
}

void beep() {
  int16_t buf[80];
  for (int i = 0; i < 40; i++) {
    int16_t s = (int16_t)(8000 * sin(2 * PI * i / 40.0));
    buf[2 * i] = s;
    buf[2 * i + 1] = s;
  }
  size_t w;
  for (int n = 0; n < 100; n++) i2s_write(I2S_NUM_1, buf, sizeof(buf), &w, 100);
}

int readMicLevel() {
  static int32_t m[256];
  size_t r = 0;
  i2s_read(I2S_NUM_0, m, sizeof(m), &r, 100);
  int32_t peak = 0;
  for (size_t i = 0; i < r / 4; i++) {
    int32_t v = abs(m[i] >> 14);
    if (v > peak) peak = v;
  }
  return (int)peak;
}

void drawScreen(int level, bool beeping) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ESP32 all-in-one test");
  display.setCursor(0, 16);
  display.print("Mic: ");
  display.println(level);
  int w = constrain(map(level, 0, 2000, 0, 128), 0, 128);
  display.drawRect(0, 30, 128, 10, SSD1306_WHITE);
  display.fillRect(0, 30, w, 10, SSD1306_WHITE);
  display.setCursor(0, 48);
  display.print("LED: ");
  display.print(ledOn ? "ON " : "OFF");
  display.print("  Spk: ");
  display.print(beeping ? "BEEP" : "idle");
  display.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(kLed, OUTPUT);
  Wire.begin(4, 15);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
  }
  setupI2S(I2S_NUM_0, true, 26, 25, 32);   // mic: SCK, WS, SD
  setupI2S(I2S_NUM_1, false, 14, 27, 33);  // speaker: BCLK, LRC, DIN
  display.clearDisplay();
  display.display();
  Serial.println("Setup done");
}

void loop() {
  unsigned long now = millis();

  if (now - lastBlink >= 500) {
    lastBlink = now;
    ledOn = !ledOn;
    digitalWrite(kLed, ledOn);
  }

  int level = readMicLevel();
  bool beeping = false;

  if (level > kThreshold && now - lastBeep > 1500) {
    drawScreen(level, true);
    beep();
    lastBeep = millis();
    beeping = true;
  }

  if (now - lastDraw >= 200) {
    lastDraw = now;
    drawScreen(level, beeping);
    Serial.print("Mic level: ");
    Serial.println(level);
  }
}