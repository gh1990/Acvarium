#ifndef MENU_CONTROL_H  // Directiva pentru a preveni includerea multiplă a acestui fișier header
#define MENU_CONTROL_H

// Include fișierele necesare pentru controlul LCD-ului, releelor, servo-ului, RTC-ului și senzorului ultrasonic
#include "LCDControl.h"
#include "RelayControl.h"
#include "ServoControl.h"
#include "RTCControl.h"
#include "UltrasonicControl.h"

// Definirea clasei MenuControl
class MenuControl {
private:
    enum EditMode {
        NONE,
        EDIT_HOURS,
        EDIT_MINUTES,
        EDIT_TEMP,
        EDIT_LEVEL,
        EDIT_SECONDS,    // New
        EDIT_DEGREES     // New
    };

    // Variabile pentru editare
    EditMode currentEditMode = NONE;
    bool isEditingOn = true;
    
    // Add these declarations for relay setup and editing
    int currentRelaySetupStep = 0;    // 0=RET, 1=ON, 2=OFF
    int currentEditingRelay = 0;      // Current relay being edited
    void displayRelayTimingMenu(int relayIndex);  // Method for displaying relay timing menu

    // Variables for editing state
    struct RelayTiming {
        uint8_t onHour;
        uint8_t onMinute;
        uint8_t offHour;
        uint8_t offMinute;
        bool enabled;
    };

    RelayTiming relayTimings[4] = {
        {7, 0, 18, 0, true},   // LUMI - enabled setat la true
        {25, 0, 20, 0, true},  // INCAL (temperature) - enabled setat la true
        {150, 0, 200, 0, true},// PUMP (water level) - enabled setat la true
        {8, 0, 20, 0, true}    // HRANA - enabled setat la true
    };

    // Methods for editing
    void handleValueEdit(bool increase);
    void startEditing(int step);
    void toggleEditMode();

    // Referințe către componentele care controlează LCD-ul, releele, servo-ul, RTC-ul și senzorul ultrasonic
    LCDControl& display;
    RelayControl& relayControl;
    ServoControl& servoControl;
    RTCControl& rtcControl;
    UltrasonicControl& ultrasonicControl;

    // Variabile pentru a urmări starea curentă a meniului și selecției
    int currentPage = 0; // 0 = meniul principal, 1 = ECRAN, 2 = meniul SET, etc.
    int currentMenu = 0; // Urmărește ce element din meniu este selectat în meniul principal
    int currentSetMenu = 0; // Urmărește meniul selectat în submeniul SET
    int currentManMenu = 0; // Urmărește meniul selectat în submeniul MAN
    int currentRelayMenu = 0; // Urmărește relaiul selectat în meniul de setări al releelor
    int currentServoMenu = 0; // Urmărește setările servo-ului selectate în meniul de control al servo-ului

    // Variabile noi pentru controlul regimurilor
    int currentRegMenu = 0;    // Pentru a ține evidența poziției în meniul REG
    bool autoMode = true;     // Pentru a ține evidența regimului curent (AUTO sau MAN) - setat implicit la true
    
    // Flaguri și variabile de timp pentru comportamentul de afişare a meniului
    bool showingMenuSelection = false; // Flag care indică dacă selecția meniului este afișată
    unsigned long lastActionTime = 0; // Timpul ultimei acțiuni (folosit pentru logica timeout-ului)
    static const unsigned long menuSelectionDisplayTime = 1000; // Timpul pentru a arăta selecția meniului înainte de a schimba automat

    static const int menuItemWidth = 5; // Lățimea textului unui element din meniu pe display

    // Funcție privată pentru a afișa un meniu generic (folosită de alte funcții de afișare)
    void afisareSetMenu(const char* title, const char** items, int itemCount, int currentItem);
    void afisareDataMenu(const char* currentDate, const char* currentTime, const char** items, int itemCount, int currentItem);
    void afisareManMenu(const char* title, const char** items, int itemCount, int currentItem);
    void afisareRelayMenu(const char* title, const char** items, int itemCount, int currentItem);

    // Variables for relay feedback
    bool showingRelayFeedback = false;
    unsigned long relayFeedbackTime = 0;
    static const unsigned long RELAY_FEEDBACK_DURATION = 1000; // 1 second feedback duration

    bool homScreenActive = false;
    static const unsigned long UPDATE_INTERVAL = 250;
    void updateHomScreen();

    // Adaugă aceste variabile în clasa MenuControl
private:
    struct ServoSettings {
        uint8_t seconds;    // Timpul în secunde
        uint8_t degrees;    // Unghiul în grade
    } servoSettings = {2, 90};  // Valori implicite: 2 secunde, 90 grade

    unsigned long servoStartTime = 0;
    bool servoFeeding = false;
    unsigned long servoFeedDuration = 2000;  // Default 2 seconds, will be updated based on settings

    void verificaLUMI(); // Funcție specială pentru verificarea și actualizarea releului LUMI

    // Funcție privată pentru verificarea și actualizarea stării releelor
    void verificaStareRelee();

public:
    // Constructor care inițializează MenuControl cu componentele necesare
    MenuControl(LCDControl& display, RelayControl& relayControl, ServoControl& servoControl,
                RTCControl& rtcControl, UltrasonicControl& ultrasonicControl)
        : display(display), relayControl(relayControl), servoControl(servoControl),
          rtcControl(rtcControl), ultrasonicControl(ultrasonicControl) {}

    // Funcții publice pentru a afișa diferite meniuri
    void mainMenu();   // Afișează meniul principal
    void setMenu();    // Afișează meniul SET
    void subMenuHom();  // Afișează pagina ECRAN cu distanța ultrasonică
    void manMenu();  // Afișează meniul de control al Manual
    void relayMenu();  // Afișează meniul de control al releelor
    void displayServoMenu();  // Afișează meniul de control al servo-ului
    void regMenu();  // Funcție pentru afișarea meniului REG (regim)

    // Funcții publice pentru gestionarea interacțiunii cu meniul pe baza intrării utilizatorului
    void checkMenuSelectionTimeout(unsigned long currentMillis);
    void handleClockwiseRotation();
    void handleCounterClockwiseRotation();
    void handleButtonPress();
    void toggleRelay(int relayNum);  // Schimbă starea relaiului specificat
    void toggleServo();  // Schimbă starea servo-ului

    // Functions to save and load settings from EEPROM
    void saveSettings();
    void loadSettings();

    // Funcții de acces pentru a obține starea curentă a meniului
    int getCurrentPage() const { return currentPage; }  // Obține indexul paginii curente
    int getCurrentSetMenu() const { return currentSetMenu; }
    bool isShowingMenuSelection() const { return showingMenuSelection; }  // Verifică dacă selecția meniului este afișată

    void update();  // Remove the inline implementation

    // Funcție pentru afișarea regimului curent
    void displayRegime();  // Declarație simplă

    // Metodă publică pentru a verifica starea releelor din exterior
    void checkRelayStates();

    // Metodă pentru inițializarea releelor la pornire
    void initializeRelays();
};

#endif  // Sfârșitul protecției pentru includerea fișierului
