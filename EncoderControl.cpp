#include "EncoderControl.h"  // Include fișierul de antet pentru controlul encoderului
#include <Arduino.h>  // Include biblioteca principală pentru platforma Arduino

// Funcția de inițializare pentru encoder
void EncoderControl::begin() {
    pinMode(pinA, INPUT_PULLUP);  // Setează pinul A al encoderului ca intrare cu rezistență internă la 5V
    pinMode(pinB, INPUT_PULLUP);  // Setează pinul B al encoderului ca intrare cu rezistență internă la 5V
    pinMode(buttonPin, INPUT_PULLUP);  // Setează pinul butonului encoderului ca intrare cu rezistență internă la 5V
    lastPinAState = digitalRead(pinA);  // Citește și stochează starea inițială a pinului A
}

// Funcția pentru a verifica rotația encoderului (sensul de rotație)
bool EncoderControl::checkRotation(bool& clockwise) {
    unsigned long currentMillis = millis();  // Salvează timpul curent în milisecunde
    int currentPinA = digitalRead(pinA);  // Citește starea curentă a pinului A
    bool rotationDetected = false;  // Variabilă care semnalează dacă s-a detectat rotație

    // Dacă există o schimbare de la HIGH la LOW pe pinul A (indică o rotație)
    if ((lastPinAState == HIGH) && (currentPinA == LOW)) {
        // Dacă a trecut suficient timp de la ultima rotire (debounce)
        if (currentMillis - lastEncoderTime >= encoderDebounceTime) {
            clockwise = (digitalRead(pinB) == HIGH);  // Determină sensul rotației
            rotationDetected = true;  // Marchează rotația ca fiind detectată
        }
        lastEncoderTime = currentMillis;  // Actualizează timpul ultimei rotații
    }
    lastPinAState = currentPinA;  // Salvează starea curentă a pinului A
    return rotationDetected;  // Returnează dacă s-a detectat rotația
}

// Funcția pentru a verifica apăsarea butonului encoderului
bool EncoderControl::checkButton() {
    int reading = digitalRead(buttonPin);  // Citește starea curentă a butonului
    unsigned long currentMillis = millis();  // Salvează timpul curent în milisecunde
    bool buttonPressed = false;  // Variabilă care semnalează dacă butonul a fost apăsat

    // Dacă starea butonului s-a schimbat, actualizează timpul pentru debounce
    if (reading != lastButtonState) {
        lastDebounceTime = currentMillis;
    }

    // Dacă a trecut suficient timp de la ultima schimbare de stare (debounce)
    if ((currentMillis - lastDebounceTime) > buttonDebounceDelay) {
        // Dacă starea butonului s-a schimbat, actualizează starea și verifică dacă a fost apăsat
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW) {  // Dacă butonul este apăsat (stare LOW)
                buttonPressed = true;
            }
        }
    }
    lastButtonState = reading;  // Salvează starea curentă a butonului
    return buttonPressed;  // Returnează dacă butonul a fost apăsat
}
