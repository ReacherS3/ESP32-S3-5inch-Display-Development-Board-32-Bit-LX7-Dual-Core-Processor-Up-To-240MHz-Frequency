
# Waveshare-5-Inch-Development-Board

This repository provides example files and configuration templates for the **Waveshare 5-Inch Capacitive Touch Display Development Board** based on the ESP32-S3.

## 📁 Repository Purpose

This repository serves as a working environment for development, testing, and integration of custom firmware, drivers, and UI interfaces for the 5-inch Waveshare display board.

Feel free to:
- ✏️ Modify and expand existing code to suit your needs.
- 📚 Add new features or modules relevant to your specific use case.
- 🧪 Experiment with LVGL, touch controllers (e.g., GT911), display drivers (e.g., ST7262), RS485 communication, or any other peripherals.
- 🤝 Contribute with improvements, new examples, or documentation.

## ✅ Why This Repo?

This repo is designed to be:

- **Flexible** – Build your own features on top of the base.
- **Collaborative** – Anyone with permission can add or update content.
- **Modular** – Easily test one feature without breaking others.

## 🧑‍💻 Maintainer & Contact

- Name: Gert Coetser
- Email: gert@semper-fi.co.za
- Contact: +27 79 663 4288

---

## 🎓 SquareLine Studio Tutorial

This section provides a basic guide for getting started with [SquareLine Studio](https://squareline.io/), which is used to design and export user interfaces for embedded displays like the 5-Inch Waveshare board.

### 🔧 Setup & Installation

1. **Download SquareLine Studio** from the official [website](https://squareline.io/).
2. Install the software and choose a **screen resolution** matching your board (e.g., 800x480 or 1024x600).
3. Configure the **project settings** to match your microcontroller (ESP32-S3).

### 🖌️ UI Design

- Drag and drop UI elements (e.g., buttons, labels, images).
- Organize your design using **screens**, **containers**, and **layers**.
- Customize the styles using the **style tab** on the right panel.

### 🚀 Exporting the UI

1. After completing your design, go to **Project > Export UI Files**.
2. Select **LVGL** as the output format.
3. Copy the exported `ui.c`, `ui.h`, and `ui_assets.c/h` into your Arduino or PlatformIO project.

### 🧩 Integration with ESP32-S3

- Include the generated files in your main `.ino` project.
- Initialize the LVGL UI with:

```cpp
#include "ui.h"

void setup() {
  lvgl_setup();  // Your display driver setup
  ui_init();     // Initialize UI from SquareLine
}
```

- Make sure your project uses the correct LVGL version (as used in SquareLine).

### 📌 Notes

- LVGL version compatibility is key (SquareLine uses LVGL 8+).
- Always check touch controller and screen initialization compatibility.
