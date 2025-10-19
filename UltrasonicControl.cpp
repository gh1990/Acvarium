#include "UltrasonicControl.h"
#include <Arduino.h>

// Funcția de inițializare a senzorului ultrasonic
void UltrasonicControl::begin() {
    pinMode(trigPin, OUTPUT);  // Setează pinul de trig ca OUTPUT
    pinMode(echoPin, INPUT);   // Setează pinul de echo ca INPUT
}

// Funcția privată de măsurare a distanței folosind senzorul ultrasonic
void UltrasonicControl::measure() {
    // Trimite un semnal de 10 microsecunde pe pinul de trig
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Măsoară durata semnalului de la pinul de echo
    duration = pulseIn(echoPin, HIGH);
    
    // Calculează distanța în mm pe baza duratei semnalului și vitezei sunetului (0.343 cm/microsec)
    distance = (duration * 0.343 / 2);
}

// Funcția de actualizare a măsurătorii (se execută doar dacă este timpul pentru o nouă măsurătoare)
bool UltrasonicControl::update() {
    unsigned long currentMillis = millis();
    // Verifică dacă a trecut intervalul de măsurare stabilit
    if (currentMillis - lastMeasurement > measurementInterval) {
        measure();  // Realizează măsurătoarea
        lastMeasurement = currentMillis;  // Actualizează timpul ultimei măsurători
        return true;  // Semnalează că a fost realizată o nouă măsurătoare
    }
    return false;  // Nu este timpul pentru o nouă măsurătoare
}

// Funcția de obținere a distanței filtrate (media ultimelor 5 măsurători)
int UltrasonicControl::getFilteredDistance() {
    static int lastDistances[5] = {0};  // Array pentru ultimele 5 distanțe măsurate
    static int index = 0;  // Indexul pentru a înlocui valorile în array

    // Salvează ultima distanță măsurată
    lastDistances[index] = distance;
    index = (index + 1) % 5;  // Incrementează indexul, revenind la 0 după 5

    // Calculează media ultimelor 5 distanțe
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += lastDistances[i];
    }

    // Returnează media celor 5 valori
    return sum / 5;
}
