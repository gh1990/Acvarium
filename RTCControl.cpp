#include "RTCControl.h"        // Includem fișierul header care declară clasa RTCControl
#include <Arduino.h>           // Biblioteca de bază Arduino (pentru funcții precum millis(), snprintf etc.)

// ============================
// Inițializare modul RTC
// ============================
bool RTCControl::begin() {
    // Încearcă să initializeze modulul RTC prin I2C
    if (!rtc.begin()) {
        return false;          // Dacă RTC-ul nu răspunde, întoarce false (eroare)
    }

    // Verifică dacă modulul RTC a pierdut alimentarea (bateria a fost deconectată)
    if (rtc.lostPower()) {
        // Dacă da, setează ora curentă la momentul compilării codului (folosind macro-urile __DATE__ și __TIME__)
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    return true;               // RTC-ul a fost inițializat cu succes
}

// ============================
// Actualizare dată și oră
// ============================
bool RTCControl::update() {
    // Obține timpul actual în milisecunde de la pornirea plăcii (folosit pentru temporizare fără delay())
    unsigned long currentMillis = millis();

    // Verifică dacă a trecut un interval minim (ex. 1000 ms) de la ultima actualizare
    if (currentMillis - lastTimeCheck >= timeCheckInterval) {

        // Citește ora și data curentă de la modulul RTC
        DateTime now = rtc.now();

        // Verifică dacă ora sau minutul s-a schimbat față de ultima actualizare
        if (now.hour() != lastHour || now.minute() != lastMinute) {
            // Salvează noile valori ale orei și minutului
            lastHour = now.hour();
            lastMinute = now.minute();

            // Formatează data curentă ca șir de caractere în formatul "DD.MM.YY"
            snprintf(currentDate, sizeof(currentDate), "%02d.%02d.%02d",
                     now.day(), now.month(), now.year() % 100);

            // Formatează ora curentă ca șir în formatul "HH:MM"
            snprintf(currentTime, sizeof(currentTime), "%02d:%02d",
                     now.hour(), now.minute());

            // Reține momentul ultimei actualizări
            lastTimeCheck = currentMillis;

            return true;       // Semnalează că data/ora au fost actualizate
        }

        // Dacă ora/minutul nu s-au schimbat, doar actualizează timerul intern
        lastTimeCheck = currentMillis;
    }

    return false;              // Nu s-a produs nicio modificare relevantă a timpului
}

float RTCControl::getTemperature() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastTempRead >= TEMP_READ_INTERVAL) {
        lastTemp = rtc.getTemperature(); // Using the existing rtc object
        lastTempRead = currentMillis;
    }
    return lastTemp;
}
