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

