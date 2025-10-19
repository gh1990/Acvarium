#ifndef RELAY_CONTROL_H  // Verifică dacă fișierul nu a fost deja inclus
#define RELAY_CONTROL_H  // Definește macro-ul pentru a preveni includerea multiplă

#include <Arduino.h>  // Include biblioteca principală pentru platforma Arduino
#include <EEPROM.h>   // Include biblioteca pentru manipularea memoriei EEPROM

class RelayControl {  // Definirea clasei RelayControl pentru gestionarea releelor
private:
    static const int MAX_RELAYS = 4;       // Numarul maxim de relee suportate de sistem
    int relayPins[MAX_RELAYS];             // Array care conține pinii asignați fiecărui releu
    bool relayStates[MAX_RELAYS];          // Array care ține starea fiecărui releu (on/off)
    int relayCount;                        // Numarul efectiv de relee utilizate în sistem

public:
    // Constructorul clasei RelayControl - primește array-ul de pini și numărul de relee
    RelayControl(const int pins[], int count);

    // Inițializează pinii releelor ca OUTPUT și îi setează pe LOW (adică dezactivați)
    void begin();

    // Comută starea unui releu specificat (activ/relaționat)
    void toggle(int relayNum);

    // Obține starea curentă a unui releu (TRUE dacă este pornit, FALSE dacă este oprit)
    bool getState(int relayNum) const;

    // Returnează numărul total de relee configurate
    int getCount() const;

    // Setează direct starea unui releu specificat (pornit sau oprit)
    void setState(int relayNum, bool state);
};

#endif  // Sfârșitul protecției pentru includerea multiplă
