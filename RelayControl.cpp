#include "RelayControl.h"  // Include fișierul header pentru clasa RelayControl

// Constructorul clasei RelayControl - primește un array cu pini și numărul de relee
// Inițializează pinii și stările releelor
RelayControl::RelayControl(const int pins[], int count) {
    // Setează numărul de relee, având în vedere limita MAX_RELAYS
    relayCount = (count <= MAX_RELAYS) ? count : MAX_RELAYS;
    
    // Asociază pinii și setează stările inițiale ale releelor (oprite)
    for (int i = 0; i < relayCount; i++) {
        relayPins[i] = pins[i];
        relayStates[i] = false;  // Toate releele sunt oprite la început
    }
}

// Inițializează pinii releelor ca OUTPUT și setează stările lor pe LOW (dezactivat)
void RelayControl::begin() {
    for (int i = 0; i < relayCount; i++) {
        pinMode(relayPins[i], OUTPUT);            // Setează pinii ca ieșiri
        digitalWrite(relayPins[i], LOW);          // Setează starea releelor pe LOW (oprit)
    }
}

// Comută starea unui releu specificat (activează sau dezactivează)
void RelayControl::toggle(int relayNum) {
    // Verifică dacă numărul releului este valid
    if (relayNum >= 0 && relayNum < relayCount) {
        // Comută starea (dacă era oprit, pornește-l și viceversa)
        relayStates[relayNum] = !relayStates[relayNum];
        digitalWrite(relayPins[relayNum], relayStates[relayNum] ? HIGH : LOW);  // Actualizează starea pinului
    }
}

// Obține starea curentă a unui releu (adevărat dacă este pornit, fals dacă este oprit)
bool RelayControl::getState(int relayNum) const {
    // Verifică dacă numărul releului este valid
    if (relayNum >= 0 && relayNum < relayCount) {
        return relayStates[relayNum];  // Returnează starea actuală a releului
    }
    return false;  // Dacă numărul releului nu este valid, returnează false
}

// Returnează numărul total de relee disponibile
int RelayControl::getCount() const {
    return relayCount;  // Returnează numărul de relee configurate
}

// Setează direct starea unui releu specificat (pornește sau oprește)
void RelayControl::setState(int relayNum, bool state) {
    // Verifică dacă numărul releului este valid
    if (relayNum >= 0 && relayNum < relayCount) {
        relayStates[relayNum] = state;  // Actualizează starea interne a releului
        digitalWrite(relayPins[relayNum], state ? HIGH : LOW);  // Actualizează starea pinului
    }
}
