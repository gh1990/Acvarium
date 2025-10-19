#include "ServoControl.h"  // Include fișierul header pentru clasa ServoControl
#include <Arduino.h>        // Include biblioteca Arduino pentru funcționalități de bază

// Funcția de inițializare a servo-ului
void ServoControl::begin() {
    myServo.attach(servoPin);  // Leagă servo-ul la pinul specificat
    myServo.write(servoInitialPosition);  // Setează poziția inițială a servo-ului
}

// Funcția de comutare a stării servo-ului (pornit/oprit)
void ServoControl::toggle() {
    servoState = !servoState;  // Comută starea servo-ului (de la pornit la oprit sau invers)
    if (servoState) {
        myServo.write(servoActivePosition);  // Dacă este pornit, setează poziția activă
    } else {
        myServo.write(servoInitialPosition);  // Dacă este oprit, setează poziția inițială
    }
    isServoMoving = true;  // Marchează servo-ul ca fiind în mișcare
    servoMoveTime = millis();  // Salvează momentul când a început mișcarea
}

// Funcția de actualizare a stării servo-ului
void ServoControl::update() {
    unsigned long currentTime = millis();
    
    // Actualizare pentru mișcare normală
    if (isServoMoving && (currentTime - servoMoveTime >= servoMoveDuration)) {
        isServoMoving = false;
    }
    
    // Actualizare pentru mișcare graduală
    if (isMovingGradual && (currentTime - lastMoveTime >= 20)) {
        int currentPosition = myServo.read();
        
        if (currentPosition < currentTargetPosition) {
            currentPosition = min(currentPosition + moveSpeed, currentTargetPosition);
            myServo.write(currentPosition);
        } else if (currentPosition > currentTargetPosition) {
            currentPosition = max(currentPosition - moveSpeed, currentTargetPosition);
            myServo.write(currentPosition);
        }
        
        if (currentPosition == currentTargetPosition) {
            isMovingGradual = false;
        }
        
        lastMoveTime = currentTime;
    }
}

// Funcția de setare a poziției servo-ului într-o valoare specificată
void ServoControl::setPosition(int degrees) {
    // Verifică dacă valoarea este între 0 și 180 grade
    if (degrees >= 0 && degrees <= 180) {
        myServo.write(degrees);  // Setează poziția servo-ului
    }
}

// Înlocuim funcția moveGradual existentă
void ServoControl::moveGradual(int targetPosition, int speed) {
    currentTargetPosition = targetPosition;
    moveSpeed = speed;
    isMovingGradual = true;
    lastMoveTime = millis();
}
