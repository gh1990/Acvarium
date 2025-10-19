#ifndef RTC_CONTROL_H
#define RTC_CONTROL_H

// Includerea bibliotecii pentru RTC (Real Time Clock) DS3231 și biblioteca Wire pentru comunicația I2C
#include <RTClib.h>
#include <Wire.h>

// Definirea clasei RTCControl pentru gestionarea unui ceas RTC
class RTCControl {
private:
    RTC_DS3231 rtc;  // Already existing RTC object
    char currentDate[11];         // Buffer pentru stocarea datei curente (format DD.MM.YY)
    char currentTime[6];          // Buffer pentru stocarea orei curente (format HH:MM)
    int lastHour = -1;            // Variabilă pentru stocarea ultimei ore procesate
    int lastMinute = -1;          // Variabilă pentru stocarea ultimului minut procesat
    unsigned long lastTimeCheck = 0;   // Timpul ultimei verificări a RTC-ului
    const unsigned long timeCheckInterval = 1000;  // Intervalul de verificare al RTC-ului (în milisecunde)
    float lastTemp = 0.0f;
    unsigned long lastTempRead = 0;
    static const unsigned long TEMP_READ_INTERVAL = 1000;

public:
    RTCControl() {}  // Constructorul clasei RTCControl (nu face nimic aici)

    // Inițializează RTC-ul, returnează true dacă a fost inițializat cu succes
    bool begin();

    // Actualizează datele și ora curentă
    bool update();

    // Returnează data curentă sub formă de șir (DD.MM.YY)
    const char* getDate() const { return currentDate; }

    // Returnează ora curentă sub formă de șir (HH:MM)
    const char* getTime() const { return currentTime; }

    // Returnează obiectul DateTime care conține data și ora curente ale RTC-ului
    DateTime getNow() { return rtc.now(); }

    // Ajustează RTC-ul la data și ora specificată
    void adjust(const DateTime& dt) { rtc.adjust(dt); }

    // Only declare the method, implementation goes in cpp file
    float getTemperature();
};

#endif
