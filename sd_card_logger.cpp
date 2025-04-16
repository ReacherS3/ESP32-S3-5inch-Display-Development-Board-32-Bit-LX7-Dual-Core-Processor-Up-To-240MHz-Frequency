#include "sd_card_logger.h"
#include <lvgl.h>
#include "ui.h"  // SquareLine Studio UI header

// SPI instance
SPIClass spiSD(HSPI);

bool initSDCard() {
  if (expander) {
    expander->digitalWrite(SD_CS, LOW);
    delay(10);
  } else {
    Serial.println("ERROR: IO Expander not initialized!");
    return false;
  }

  spiSD.begin(SD_CLK, SD_MISO, SD_MOSI, -1);

  if (!SD.begin(-1, spiSD)) {
    Serial.println("SD Card Mount Failed!");
    return false;
  }

  Serial.println("SD Card initialized successfully.");
  return true;
}

bool logTemperatureToSD(float temp1, float temp2, float temp3, float temp4, const char* paStatus, const char* dbStatus, const char* reason) {
  datetime_t now;
  PCF85063A_Read_now(&now);

  char dateStr[16];
  snprintf(dateStr, sizeof(dateStr), "%04d-%02d-%02d", now.year, now.month, now.day);

  char timeStr[16];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", now.hour, now.min, now.sec);

  char shortTimeStr[8];
  snprintf(shortTimeStr, sizeof(shortTimeStr), "%02d:%02d", now.hour, now.min);

  String csvLine = String(dateStr) + "," +
                   String(timeStr) + "," +
                   String(temp1, 2) + "," +
                   String(temp2, 2) + "," +
                   String(temp3, 2) + "," +
                   String(temp4, 2) + "," +
                   String(paStatus) + "," +
                   String(dbStatus) + "," +
                   String(reason);

  char filename[32];
  snprintf(filename, sizeof(filename), "/log_%04d-%02d-%02d.csv", now.year, now.month, now.day);

  File file = SD.open(filename, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending.");
    return false;
  }

  if (file.size() == 0) {
    file.println("Date,Time,Temp1,Temp2,Temp3,Temp4,PA Pressure,DB Pressure,Reason");
  }

  file.println(csvLine);
  file.close();

  Serial.println("Logged to SD: " + csvLine);

  if (lv_obj_get_child_cnt(ui_logPanel) >= 3) {
    lv_obj_del(lv_obj_get_child(ui_logPanel, 0));
  }

  String uiLine1 = String(shortTimeStr) + "  | PA: " + paStatus + " | DB: " + dbStatus + " | Status: " + reason;
  String uiLine2 = "      Zone 1: " + String(temp1, 1) + "\u00B0C\n" +
                   "      Zone 2: " + String(temp2, 1) + "\u00B0C\n" +
                   "      Zone 3: " + String(temp3, 1) + "\u00B0C\n" +
                   "      Zone 4: " + String(temp4, 1) + "\u00B0C";

  lv_obj_t *logLabel = lv_label_create(ui_logPanel);
  lv_label_set_text_fmt(logLabel, "%s\n%s", uiLine1.c_str(), uiLine2.c_str());

  lv_obj_scroll_to_view(logLabel, LV_ANIM_ON);

  return true;
}
