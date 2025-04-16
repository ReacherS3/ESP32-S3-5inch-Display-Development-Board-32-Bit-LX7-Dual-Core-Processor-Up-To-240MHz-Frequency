#include <Wire.h>
#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>
#include <lvgl.h>
#include <ui.h>
#include "lvgl_port_v8.h"
#include "loading_bar.h"
#include "waveshare_pcf85063a.h"
#include "twai_functions.h"
#include "rs485_functions.h"
#include "wifi_functions.h"
#include "influxdb_functions.h"
#include "sd_card_logger.h"                                                                                             //SD in the functions

ESP_IOExpander_CH422G *expander = NULL;

#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

datetime_t Now_time;
unsigned long lastUpdateTime = 0;
char time_str[16];
char date_str[16];


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

  Serial.println("Initialize panel device");
  ESP_Panel *panel = new ESP_Panel();
  panel->init();

#if LVGL_PORT_AVOID_TEAR
  ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
  rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
  rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
#endif

  panel->begin();
  Serial.println("Initialize LVGL");
  lvgl_port_init(panel->getLcd(), panel->getTouch());

  Serial.println("Create UI");
  lvgl_port_lock(-1);
  ui_init();
  lvgl_port_unlock();

  if (ui_btn4 != NULL) {
    lv_obj_add_event_cb(ui_btn4, ui_btn4_event_handler, LV_EVENT_CLICKED, NULL);
  }
  if (ui_btn1 != NULL) {
    lv_obj_add_event_cb(ui_btn1, ui_btn1_event_handler, LV_EVENT_CLICKED, NULL);
  }
  if (ui_btn_disconnect != NULL) {
    lv_obj_add_event_cb(ui_btn_disconnect, ui_btn_disconnect_event_handler, LV_EVENT_CLICKED, NULL);
  }
  
  initLoadingBar();
  initializeRTC();
  initializeWiFi();
  initializeTWAI();
  initRS485(); 
  initSDCard();                                                                               //SD in the Setup

  if (WiFi.status() == WL_CONNECTED) {
    initializeInfluxDB();
  }

  Serial.println("Setup complete.");
}

void loop() {
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

  if (currentMillis - lastSDWrite >= 10000) {
    lastSDWrite = currentMillis;
    Serial.println("Logging temperature to SD card...");

    logTemperatureToSD(                                                                                //SD Log in the Loop
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

