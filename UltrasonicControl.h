#ifndef ULTRASONIC_CONTROL_H  // Protejează fișierul pentru a nu fi inclus de mai multe ori
#define ULTRASONIC_CONTROL_H

#include <Arduino.h>  // Include biblioteca Arduino pentru funcționalități de bază

// Clasa pentru controlul senzorului ultrasonic
class UltrasonicControl {
private:
    const int trigPin;            // Pinul de trigger al senzorului ultrasonic
    const int echoPin;            // Pinul de echo al senzorului ultrasonic
    long duration;                // Durata semnalului de la trigger la echo (în microsecunde)
    int distance;                 // Distanța măsurată (în mm)
    unsigned long lastMeasurement = 0;  // Momentul ultimei măsurători
    const unsigned long measurementInterval = 500;  // Intervalul minim între măsurători (în milisecunde)

    // Funcția privată pentru realizarea măsurării
    void measure();

public:
    // Constructorul clasei, primește pini pentru trig și echo
    UltrasonicControl(int trig, int echo) : trigPin(trig), echoPin(echo) {}

    // Funcția de inițializare a senzorului ultrasonic
    void begin();

    // Funcția de actualizare a măsurătorii și verificare dacă este timpul pentru o nouă măsurătoare
    bool update();

    // Funcția de obținere a distanței măsurate
    int getDistance() const { return distance; }

    // Funcția de obținere a unei distanțe filtrate (potențial pentru a reduce erorile de măsurare)
    int getFilteredDistance();
};

#endif  // Sfârșitul protecției împotriva includerii multiple
