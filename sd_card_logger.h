#ifndef SD_CARD_LOGGER_H
#define SD_CARD_LOGGER_H

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <ESP_IOExpander_Library.h>
#include "waveshare_pcf85063a.h"

// SPI pin definitions
#define SD_MOSI 11
#define SD_CLK  12
#define SD_MISO 13
#define SD_CS 4  // Controlled via IO Expander

extern ESP_IOExpander_CH422G* expander;

bool initSDCard();

// Updated logger with PA and DB pressure included
bool logTemperatureToSD(float temp1, float temp2, float temp3, float temp4,
                        const char* paStatus, const char* dbStatus,
                        const char* reason);

#endif // SD_CARD_LOGGER_H