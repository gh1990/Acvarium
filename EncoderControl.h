#ifndef ENCODER_CONTROL_H  // Verifică dacă fișierul nu a fost deja inclus
#define ENCODER_CONTROL_H  // Definește macro-ul pentru a preveni includerea multiplă

#include <Arduino.h>  // Include biblioteca principală pentru platforma Arduino

class EncoderControl {  // Definirea clasei EncoderControl pentru gestionarea encoderului
private:
    const int pinA;  // Pinul A al encoderului (semnal de ceas)
    const int pinB;  // Pinul B al encoderului (semnal de direcție)
    const int buttonPin;  // Pinul butonului encoderului
    int lastPinAState = LOW;  // Starea anterioară a pinului A (inițial LOW)
    int lastButtonState = HIGH;  // Starea anterioară a butonului (inițial HIGH)
    int buttonState = HIGH;  // Starea curentă a butonului (inițial HIGH)
    unsigned long lastEncoderTime = 0;  // Timpul ultimei rotații a encoderului
    unsigned long lastDebounceTime = 0;  // Timpul ultimei schimbări de stare a butonului pentru debounce
    const unsigned long encoderDebounceTime = 10;  // Timpul minim între două rotații pentru a evita bouncing-ul
    const unsigned long buttonDebounceDelay = 50;  // Timpul minim între două apăsări de buton pentru a evita bouncing-ul

public:
    // Constructorul clasei EncoderControl, care primește pinii pentru semnalele encoderului
    EncoderControl(int clkPin, int dtPin, int swPin)
        : pinA(clkPin), pinB(dtPin), buttonPin(swPin) {}

    // Inițializează pinurile pentru encoder și buton
    void begin();

    // Verifică rotația encoderului și returnează sensul rotației (sensul ceasornicului sau invers)
    bool checkRotation(bool& clockwise);

    // Verifică apăsarea butonului encoderului
    bool checkButton();
};

#endif  // Sfârșitul protecției pentru includerea multiplă
