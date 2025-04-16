#include "sd_card_logger.h"


#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5


void setup() {
  Serial.begin(115200);
  Wire.begin();
  scanI2CDevices();
  pinMode(GPIO_INPUT_IO_4, OUTPUT);

  Serial.println("Initialize IO expander");
  expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
  expander->init();
  expander->begin();
  expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL, OUTPUT);
  expander->multiDigitalWrite(TP_RST | LCD_BL | LCD_RST, HIGH);

  expander->digitalWrite(SD_CS, LOW);
  expander->digitalWrite(TP_RST, HIGH);
  expander->digitalWrite(LCD_RST, HIGH);
  expander->digitalWrite(LCD_BL, HIGH);
  delay(100);

  expander->digitalWrite(USB_SEL, LOW);
  delay(2000);
  expander->digitalWrite(TP_RST, LOW);
  delay(100);
  digitalWrite(GPIO_INPUT_IO_4, LOW);
  delay(100);
  expander->digitalWrite(TP_RST, HIGH);
  delay(200);



  initSDCard();





  void loop() {

    /*
  static unsigned long lastRTCTimeUpdate = 0;
  static unsigned long lastProgressUpdate = 0;
  static unsigned long lastInfluxDBSend = 0;
  static unsigned long lastSDWrite = 0;
  static unsigned long lastIdlePrint = 0;
  static unsigned long lastLoop = 0;

  unsigned long currentMillis = millis();

  unsigned long loopDuration = currentMillis - lastLoop;
  lastLoop = currentMillis;
  Serial.printf("Loop duration: %lu ms\n", loopDuration);

  if (currentMillis - lastRTCTimeUpdate >= 1000) {
    lastRTCTimeUpdate = currentMillis;
    PCF85063A_Read_now(&Now_time);
    updateUIWithRTC();
  }
*/
  /*
  handleRS485();
  handleWiFi();

  if (currentMillis - lastProgressUpdate >= 150) {
    lastProgressUpdate = currentMillis;
    simulateLoadingProgress();
  }

  if (WiFi.status() == WL_CONNECTED && currentMillis - lastInfluxDBSend >= 10000) {
    lastInfluxDBSend = currentMillis;
    Serial.println("Sending RS485 data to InfluxDB...");
    sendToInfluxDB(temperatures[0], temperatures[1], temperatures[2], temperatures[3]);
  }
*/
  if (currentMillis - lastSDWrite >= 10000) {
    lastSDWrite = currentMillis;
    Serial.println("Logging temperature to SD card...");

    logTemperatureToSD(
      temperatures[0],
      temperatures[1],
      temperatures[2],
      temperatures[3],
      String(receivedPressure, 2).c_str(),
      pressureStatus.c_str(),
      "Normal"
    );
  }

  if (currentMillis - lastIdlePrint >= 1000) {
    Serial.println("IDLE loop");
    lastIdlePrint = currentMillis;
  }

  delay(150);
}

