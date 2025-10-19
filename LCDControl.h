#ifndef LCD_CONTROL_H
#define LCD_CONTROL_H

#include <LiquidCrystal_I2C.h>

class LCDControl {
private:
    LiquidCrystal_I2C lcd;

public:
    LCDControl() : lcd(0x27, 16, 2) {}
    
    // Basic functions - declare only, implement in cpp
    void begin();
    void clear() { lcd.clear(); }
    void setCursor(int col, int row) { lcd.setCursor(col, row); }
    
    // Print functions - keep inline
    void print(const char* text) { lcd.print(text); }
    void print(int value) { lcd.print(value); }
    void print(char c) { lcd.print(c); }
    void print(float value, int precision) { lcd.print(value, precision); }
    void print(const __FlashStringHelper *ifsh) { lcd.print(ifsh); }
    
    // Custom character functions - keep inline
    void createChar(uint8_t location, const uint8_t charmap[]) {
        lcd.createChar(location, charmap);
    }
    void write(uint8_t value) { lcd.write(value); }
    
    // Display specific functions - declare only, implement in cpp
    void showStartupScreen(const char* version, const char* author);
    void showMessageTwoLines(const char* line1, const char* line2);
    void showmainMenu(const char* date, const char* time, const char** menuItems,
                     int currentItem, int menuCount, int menuItemWidth);
    void showRelayStatus(int relayNum, bool state);
    void showServoStatus(bool state);
    void showDistance(int distance);
};

#endif
