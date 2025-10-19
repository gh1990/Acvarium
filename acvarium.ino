// Includere biblioteci necesare
#include <Wire.h>              // Biblioteca pentru comunicare I2C (folosită de RTC și LCD)
#include <RTClib.h>            // Biblioteca pentru lucrul cu ceasul în timp real DS3231/DS1307
#include <Servo.h>             // Biblioteca standard pentru controlul servo-motoarelor
#include "ServoControl.h"      // Clasa personalizată pentru controlul servo-ului
#include "RelayControl.h"      // Clasa personalizată pentru controlul releelor
#include "RTCControl.h"        // Clasa personalizată pentru controlul RTC-ului
#include "UltrasonicControl.h" // Clasa personalizată pentru senzorul de distanță ultrasonic
#include "LCDControl.h"        // Clasa personalizată pentru afișajul LCD
#include "EncoderControl.h"    // Clasa personalizată pentru encoderul rotativ
#include "MenuControl.h"       // Clasa personalizată pentru logica de meniu

// Inițializare obiect pentru afișajul LCD
LCDControl display;




// Configurare pini pentru encoder rotativ
const int encoderPinA = A1;    // Pinul conectat la CLK (canal A) al encoderului
const int encoderPinB = A2;    // Pinul conectat la DT (canal B) al encoderului
const int encoderButton = A3;  // Pinul conectat la butonul encoderului

// Inițializare instanță encoder
EncoderControl encoder(encoderPinA, encoderPinB, encoderButton);

// Inițializare componente hardware
ServoControl servoControl(A0);              // Servo atașat la pinul A0
UltrasonicControl ultrasonicControl(3, 4);   // Senzor ultrasonic conectat la pinii D3 (trigger) și D4 (echo)
const int relayPinsArray[] = {5, 6, 7, 8};   // Array cu pinii pentru cele 4 relee
RelayControl relayControl(relayPinsArray, 4); // Instanță pentru controlul celor 4 relee

// Variabile pentru controlul actualizărilor la intervale regulate
unsigned long lastTimeCheck = 0;                    // Ultimul moment când s-a actualizat ora
const unsigned long timeCheckInterval = 1000;       // Interval între actualizările orei, în milisecunde (1s)

// Inițializare RTC (ceas în timp real)
RTCControl rtcControl;

// Inițializare meniu principal
MenuControl menuControl(display, relayControl, servoControl, rtcControl, ultrasonicControl);

// Constants
const unsigned long BUTTON_DEBOUNCE_DELAY = 250;    // Delay pentru debounce buton în milisecunde
const unsigned long MENU_UPDATE_INTERVAL = 250;      // Interval actualizare meniu în milisecunde


void setup() {
    Serial.begin(9600);        // Pornire comunicație serială pentru debug
    Wire.begin();              // Inițializare magistrală I2C

    // Inițializare componente
    servoControl.begin();          // Inițializare servo
    if (!rtcControl.begin()) {     // Inițializare RTC, verificare dacă funcționează
        Serial.println("Nu s-a găsit modulul RTC!");  // Mesaj de eroare
        while (1);                 // Blochează execuția dacă RTC-ul lipsește
    }
    display.begin();               // Inițializare afișaj LCD
    encoder.begin();               // Inițializare encoder rotativ
    ultrasonicControl.begin();     // Inițializare senzor ultrasonic
    relayControl.begin();          // Inițializare module relee
    
    // Load saved settings from EEPROM
    menuControl.loadSettings();    // Load settings at startup

    // Inițializare relee și verificarea stării lor
    menuControl.initializeRelays();  // Păstrează doar unul din aceste apeluri

    // Afișare ecran de pornire
    displayStartupScreen();
}

void loop() {
    unsigned long currentMillis = millis();

    // Actualizare logica componentelor
    servoControl.update();
    ultrasonicControl.update();
    menuControl.update();  // Adăugat update pentru meniu

    // Verifică timeout-ul pentru meniuri
    menuControl.checkMenuSelectionTimeout(currentMillis);

    // Gestionare afișaj în funcție de pagina activă a meniului
    if (menuControl.getCurrentPage() == 0) {  // Suntem în meniul principal
        if (currentMillis - lastTimeCheck >= timeCheckInterval) {
            updateDateTime();
            lastTimeCheck = currentMillis;
        }
    }

    // Gestionare input de la utilizator
    checkEncoder();
    checkButton();
}

// Funcție care actualizează ora și data
void updateDateTime() {
    if (rtcControl.update()) { // Dacă ora a fost actualizată cu succes
        if (menuControl.getCurrentPage() == 0 && !menuControl.isShowingMenuSelection()) {
            menuControl.mainMenu(); // Afișează meniul principal dacă e permis
        }
    }
}

// Funcție care actualizează afișajul cu distanța măsurată
void updateDistanceDisplay() {
    if (ultrasonicControl.update()) {  // Dacă există o valoare nouă
        display.showDistance(ultrasonicControl.getFilteredDistance()); // Afișează distanța pe LCD
    }
}

// Funcție care verifică rotirea encoderului și direcția acesteia
void checkEncoder() {
    bool clockwise;
    if (encoder.checkRotation(clockwise)) {  // Dacă s-a detectat rotație
        if (clockwise) {
            menuControl.handleClockwiseRotation();         // Gestionează rotație în sens orar
        } else {
            menuControl.handleCounterClockwiseRotation();  // Gestionează rotație în sens antiorar
        }
    }
}

// Actualizăm funcția checkButton pentru o mai bună gestionare a apăsării butonului
void checkButton() {
    static unsigned long lastButtonPress = 0;
    unsigned long currentMillis = millis();
    
    if (encoder.checkButton() && (currentMillis - lastButtonPress > BUTTON_DEBOUNCE_DELAY)) {  // Debounce de 250ms
        menuControl.handleButtonPress();
        lastButtonPress = currentMillis;
    }
}

// Funcție pentru afișarea ecranului de pornire
void displayStartupScreen() {
    display.showStartupScreen("ACVARIUM V1.01", "*DSPAS BY GHENA*"); // Text de întâmpinare
    delay(2000); // Așteaptă 2 secunde înainte de a trece mai departe
}
