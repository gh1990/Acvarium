#include "LCDControl.h"

void LCDControl::begin() {
    lcd.init();
    lcd.backlight();
}

void LCDControl::showStartupScreen(const char* version, const char* author) {
    clear();
    setCursor(0, 0);
    print(version);
    setCursor(0, 1);
    print(author);
}

void LCDControl::showMessageTwoLines(const char* line1, const char* line2) {
    clear();
    setCursor(0, 0);
    print(line1);
    setCursor(0, 1);
    print(line2);
}

void LCDControl::showmainMenu(const char* date, const char* time, const char** menuItems,
                            int currentItem, int menuCount, int menuItemWidth) {
    clear();
    setCursor(0, 0);
    print(date);
    print(" ");
    print(time);
    
    for (int i = 0; i < menuCount; i++) {
        setCursor(i * menuItemWidth, 1);
        print(menuItems[i]);
    }
}

void LCDControl::showRelayStatus(int relayNum, bool state) {
    clear();
    setCursor(0, 0);
    print("Relay ");
    print(relayNum);
    print(": ");
    print(state ? "ON" : "OFF");
}

void LCDControl::showServoStatus(bool state) {
    clear();
    setCursor(0, 0);
    print("Servo: ");
    print(state ? "ON" : "OFF");
}

void LCDControl::showDistance(int distance) {
    clear();
    setCursor(0, 0);
    print("Distance: ");
    print(distance);
    print("mm");
}
