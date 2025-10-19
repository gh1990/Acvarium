#ifndef SERVO_CONTROL_H  // Previne includerea multiplă a fișierului
#define SERVO_CONTROL_H

#include <Servo.h>  // Include biblioteca Servo pentru controlul servo-ului

// Clasa ServoControl - controlează mișcările unui servomotor
class ServoControl {
private:
    Servo myServo;  // Obiect Servo pentru a controla servomotorul
    const int servoInitialPosition = 0;  // Poziția inițială a servo-ului (0 grade)
    const int servoActivePosition = 90;  // Poziția activă a servo-ului (90 grade)
    unsigned long servoMoveTime = 0;     // Timpul ultimei mișcări (pentru a controla durata mișcării)
    bool isServoMoving = false;          // Flag pentru a indica dacă servo-ul se află în mișcare
    const unsigned long servoMoveDuration = 500;  // Durata unei mișcări (în milisecunde)
    bool servoState = false;             // Starea curentă a servo-ului (pornit/oprit)
    int servoPin;                        // Pinul la care este conectat servo-ul
    // Adăugăm variabile noi în clasa ServoControl
private:
    unsigned long lastMoveTime = 0;
    int currentTargetPosition = 0;
    bool isMovingGradual = false;
    int moveSpeed = 0;

public:
    // Constructor - primește pinul la care este conectat servo-ul
    ServoControl(int pin) : servoPin(pin) {}

    // Inițializează servomotorul (asociază pinul și setează poziția inițială)
    void begin();

    // Comută starea servo-ului (dacă este activ sau inactiv)
    void toggle();

    // Actualizează starea servo-ului (verifică mișcarea și actualizează starea)
    void update();

    // Returnează starea curentă a servo-ului (activ sau inactiv)
    bool getState() const { return servoState; }

    // Verifică dacă servo-ul se află în mișcare
    bool isMoving() const { return isMovingGradual; }

    // Setează poziția servo-ului într-o valoare de unghi (0 - 180 grade)
    void setPosition(int degrees);

    // Mișcă servo-ul gradual la o poziție țintă cu o viteză specificată
    void moveGradual(int targetPosition, int speed);
};

#endif  // Finalul protecției pentru includerea multiplă
