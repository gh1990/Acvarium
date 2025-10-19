#include "MenuControl.h"
#include <Arduino.h>
#include "RelayControl.h"    // Include fișierul de antet pentru controlul releelor


extern RTCControl rtcControl;  // Declarație externă pentru RTCControl (ceasul în timp real)


// Inițializare liste statice cu opțiunile din meniurile principale și secundare
const char* menuItems[] = { "HOM", "SET", "REG" };         // Meniul principal (HOM, SET, REG)
const char* setItems[] = { "RET", "RELE", "SERVO" };    // Meniul de setări (RET, RELE, SERVO)
const char* relayItems[] = { "RET", "LUMI", "INCAL", "PUMP", "FILTR" };    // Meniul de setări (RET, RELE, SERVO)
const char* manItems[] = { "RET", "LUM", "INC", "PUM", "FIL", "MIN", "SAV" };    // Meniul de setări (RET, RELE, SERVO)
const char* regItems[] = { "RET", "AUTO", "MAN" };    // Noul submeniu pentru regimuri
const char* date = rtcControl.getDate();  // Format: "DD.MM.YY"
const char* time = rtcControl.getTime();  // Format: "HH:MM"
char rezultat[50];

bool autoMode = true;  // Pentru a ține evidența regimului curent (AUTO sau MAN) - setat implicit la true

// Define custom characters at the top of the file
const uint8_t arrowUp[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00000
};

const uint8_t arrowDown[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};


// Funcție este folosită pentru a afișa un meniu generic cu data și ora curentă
void MenuControl::afisareDataMenu(const char* currentDate, const char* currentTime, const char** items, int itemCount, int currentItem) {
    display.clear();                  // Curăță ecranul
    display.setCursor(0, 0);          // Setează cursorul în colțul din stânga sus
    display.print(currentDate);             // Afișează data
    display.print(" ");             // Afișează un spațiu
    display.print(currentTime);             // Afișează ora

    for (int i = 0; i < itemCount; i++) {
        int menuPos = i * menuItemWidth;   // Calculează poziția pentru fiecare item
        display.setCursor(menuPos, 1);     // Setează cursorul pe linia a doua
        display.print(i == currentItem ? '>' : ' ');  // Afișează '>' pentru itemul selectat
        display.print(items[i]);          // Afișează opțiunea de meniu
    }
}

// Funcția pentru a afișa un meniu de setare
void MenuControl::afisareSetMenu(const char* title, const char** items, int itemCount, int currentItem) {
    display.clear();                  // Curăță ecranul
    display.setCursor(0, 0);          // Setează cursorul în colțul din stânga sus
    display.print(title);             // Afișează titlul meniului

    for (int i = 0; i < itemCount; i++) {
        int menuPos = i * menuItemWidth;   // Calculează poziția pentru fiecare item
        display.setCursor(menuPos, 1);     // Setează cursorul pe linia a doua
        display.print(i == currentItem ? '>' : ' ');  // Afișează '>' pentru itemul selectat
        display.print(items[i]);          // Afișează opțiunea de meniu
    }
}

// Funcția pentru a afișa un meniu in doua linii pe display
void MenuControl::afisareRelayMenu(const char* title, const char** items, int itemCount, int currentItem) {
    display.clear();                  // Curăță ecranul LCD pentru a afișa un meniu nou
    display.setCursor(0, 0);          // Setează cursorul pe prima linie (rândul 0, coloana 0)
    display.print(title);             // Afișează titlul meniului pe prima linie

    // Parcurge toate opțiunile din meniu
    for (int i = 0; i < itemCount; i++) {
        int menuPos = i * menuItemWidth;  // Calculează poziția orizontală (coloana) pentru afișarea itemului

        // Dacă indexul este 3 sau mai mare, ajustăm poziția pentru a lăsa spațiu între grupuri
        if (i >= 3) {
            menuPos -= 10; // Mută poziția puțin spre stânga pentru a crea un decalaj vizual
        }

        // Determină linia pe care se va afișa itemul (0 = sus, 1 = jos)
        // Itemii cu index între 1 și 2 (exclusiv 0 și >=3) se afișează pe prima linie
        int line = (i > 0 && i < 3) ? 0 : 1;

        // Setează cursorul la poziția calculată (menuPos, line)
        display.setCursor(menuPos, line);

        // Afișează simbolul '>' dacă acest item este cel selectat; altfel, un spațiu
        display.print(i == currentItem ? '>' : ' ');

        // Afișează textul itemului curent din meniu
        display.print(items[i]);
    }
}

// Funcția pentru a afișa un meniu in doua linii pe display
void MenuControl::afisareManMenu(const char* title, const char** items, int itemCount, int currentItem) {
    display.clear();                  // Curăță ecranul LCD pentru a afișa un meniu nou
    display.setCursor(0, 0);          // Setează cursorul pe prima linie (rândul 0, coloana 0)
    display.print(title);             // Afișează titlul meniului pe prima linie

    // Parcurge toate opțiunile din meniu
    for (int i = 0; i < itemCount; i++) {
        int menuPos = i * (menuItemWidth-1);  // Calculează poziția orizontală (coloana) pentru afișează itemul

        // Dacă indexul este 3 sau mai mare, ajustăm poziția pentru a lăsa spațiu între grupuri
        if (i >= 4) {
            menuPos -= 12; // Mută poziția puțin spre stânga pentru a crea un decalaj vizual
        }

        // Determină linia pe care se va afișa itemul (0 = sus, 1 = jos)
        // Itemii cu index între 1 și 2 (exclusiv 0 și >=3) se afișează pe prima linie
        int line = (i > 0 && i < 4) ? 0 : 1;

        // Setează cursorul la poziția calculată (menuPos, line)
        display.setCursor(menuPos, line);

        // Afișează simbolul '>' dacă acest item este cel selectat; altfel, un spațiu
        display.print(i == currentItem ? '>' : ' ');

        // Afișează textul itemului curent din meniu
        display.print(items[i]);
    }
}

// Funcția pentru a afișa meniul principal
void MenuControl::mainMenu() {
    homScreenActive = false;  // Dezactivăm explicit ecranul HOM
    afisareDataMenu(date, time, menuItems, 3, currentMenu);
    currentPage = 0;
}

// Funcția pentru a afișa meniul de setări
void MenuControl::setMenu() {
    // Folosim funcția generică de afișare pentru consistență
    afisareSetMenu("*SET<DISPOZITIV>", setItems, 3, currentSetMenu);
    currentPage = 1;  // Set current page to SET menu
    homScreenActive = false;  // Dezactivăm explicit ecranul HOM
}

// Funcția pentru a afișa meniul de control al releelor
void MenuControl::manMenu() {
    afisareManMenu("*MAN", manItems, 7, currentManMenu);  // Afișează meniul de setări cu 3 opțiuni
    currentPage = 2;  // Setează pagina curentă la meniul de setări
}

// Funcția pentru a afișa meniul de control al releelor
void MenuControl::relayMenu() {
    homScreenActive = false;  // Dezactivăm explicit ecranul HOM
    afisareRelayMenu("*RELE", relayItems, 5, currentRelayMenu);
    currentPage = 3;  // Setează pagina curentă la meniul de setări
}

// Implementarea corectă pentru funcția regMenu()
void MenuControl::regMenu() {
    homScreenActive = false;  // Dezactivăm ecranul HOM
    
    display.clear();
    display.setCursor(0, 0);
    display.print("*REG ");
    display.print(autoMode ? "AUTO" : "MAN");  // Afișăm regimul curent în titlu
    
    // Debug - afișăm valoarea lui autoMode
    Serial.print("autoMode = ");
    Serial.println(autoMode ? "true" : "false");
    
    // Afișăm opțiunile de regim pe rândul 2
    display.setCursor(1, 1);
    display.print("RET");
    
    // Folosim metoda displayRegime() pentru a afișa regimul curent
    displayRegime();
    
    // Afișăm cursorul de selecție
    switch (currentRegMenu) {
        case 0:  // RET
            display.setCursor(0, 1);
            display.print(">");
            break;
        case 1:  // AUTO
            display.setCursor(4, 1);
            display.print(">");
            break;
        case 2:  // MAN
            display.setCursor(11, 1);
            display.print(">");
            break;
    }
    
    currentPage = 6;  // Setăm pagina curentă la meniul de regim
}

// Funcție pentru afișarea regimului curent
void MenuControl::displayRegime() {
    display.setCursor(5, 1);
    display.print(autoMode ? "*" : " ");  // Indicator pentru regimul AUTO
    display.print("AUTO");

    display.setCursor(12, 1);
    display.print(!autoMode ? "*" : " ");  // Indicator pentru regimul MAN
    display.print("MAN");
}

// Funcția pentru a afișa pagina ECRAN
void MenuControl::subMenuHom() {
    // Create custom characters only once
    display.createChar(0, arrowUp);    
    display.createChar(1, arrowDown);  
        
    // Set flags and page
    homScreenActive = true;
    currentPage = 1;
    showingMenuSelection = false;
    
    // Initial display setup
    display.clear();
    display.setCursor(0, 0);
    display.print("*HOM LIPF T");
    
    // Display RET at correct position
    display.setCursor(1, 1);
    display.print("RET");
    display.setCursor(0, 1);
    display.print(">");  // Add marker for RET
    
    // Display initial relay states
    for (int i = 0; i < 4; i++) {
        display.setCursor(5 + i, 1);
        display.write(relayControl.getState(i) ? 0 : 1);
    }
    
    display.setCursor(9, 1);
    display.print(" H");
    
    // Show initial values
    float temp = rtcControl.getTemperature();
    display.setCursor(11, 0);
    char tempStr[6];
    dtostrf(temp, 2, 1, tempStr);
    display.print(tempStr);
    display.print("C");
    
    int waterLevel = ultrasonicControl.getDistance();
    display.setCursor(11, 1);
    if (waterLevel < 100) display.print(" ");
    if (waterLevel < 10) display.print(" ");
    display.print(waterLevel);
    display.print("mm");
}

void MenuControl::updateHomScreen() {
    // Only update if HOM screen is active
    if (!homScreenActive) return;
    
    static int lastWaterLevel = -1;
    static float lastTemp = -999.0;
    static bool lastRelayStates[4] = {false};
    
    // Update temperature if changed
    float currentTemp = rtcControl.getTemperature();
    if (abs(currentTemp - lastTemp) >= 0.1) {
        display.setCursor(11, 0);
        char tempStr[6];
        dtostrf(currentTemp, 2, 1, tempStr);
        display.print(tempStr);
        display.print("C");
        lastTemp = currentTemp;
    }
    
    // Update relay states if changed
    for (int i = 0; i < 4; i++) {
        bool currentState = relayControl.getState(i);
        if (currentState != lastRelayStates[i]) {
            display.setCursor(5 + i, 1);
            display.write(currentState ? 0 : 1);  // Use custom characters
            lastRelayStates[i] = currentState;
        }
    }
    
    // Update water level if changed
    int currentWaterLevel = ultrasonicControl.getDistance();
    if (currentWaterLevel != lastWaterLevel) {
        display.setCursor(11, 1);
        if (currentWaterLevel < 100) display.print(" ");
        if (currentWaterLevel < 10) display.print(" ");
        display.print(currentWaterLevel);
        display.print("mm");
        lastWaterLevel = currentWaterLevel;
    }
}

// Modificarea metodei checkMenuSelectionTimeout() pentru a reveni corect din starea de feedback
void MenuControl::checkMenuSelectionTimeout(unsigned long currentMillis) {
    // Verifică timeout-ul pentru feedback-ul releelor
    if (showingRelayFeedback && (currentMillis - relayFeedbackTime >= RELAY_FEEDBACK_DURATION)) {
        showingRelayFeedback = false;
        
        // Verificăm dacă tocmai am salvat setările și asigurăm revenirea la modul corespunzător
        if (autoMode) {
            // Actualizăm imediat starea releelor după feedback
            verificaStareRelee();
            
            // Verifică dacă ar trebui să revenim la meniul REG sau la ecranul principal
            if (currentPage == 6) {
                regMenu(); // Revenire la meniul REG
            } else {
                mainMenu(); // Revenire la meniul principal
            }
        } else {
            manMenu(); // Revenire la meniul MAN dacă suntem în regim manual
        }
        return;
    }

    // Verificarea pentru selecția din meniu
    if (showingMenuSelection && (currentMillis - lastActionTime >= menuSelectionDisplayTime)) {
        showingMenuSelection = false;
        if (currentPage != 2) {  // Nu reafișăm meniul MAN dacă suntem deja în el
            switch (currentPage) {
                case 0: mainMenu(); break;
                case 1: setMenu(); break;
                case 3: relayMenu(); break;
                case 4: displayServoMenu(); break;
                case 6: regMenu(); break;  // Adăugăm regMenu pentru timeout
                default: break;
            }
        }
    }
}

void MenuControl::handleClockwiseRotation() {
    if (currentEditMode != NONE) {
        handleValueEdit(true);
        return;
    }
    
    // Check if we're in HOM screen, but only block if we're actually in HOM view
    if (homScreenActive && currentMenu == 0 && currentPage == 1) {
        return;
    }
    
    switch (currentPage) {
        case 0:  // Main menu
            currentMenu = (currentMenu + 1) % 3;
            mainMenu();
            break;
        case 1:  // SET menu
            currentSetMenu = (currentSetMenu + 1) % 3;
            setMenu();
            break;
        case 2:  // MAN menu
            currentManMenu = (currentManMenu + 1) % 7;
            manMenu();
            break;
        case 3:  // RELAY menu
            currentRelayMenu = (currentRelayMenu + 1) % 5;
            relayMenu();
            break;
        case 4:  // Servo menu
            if (currentEditMode != NONE) {
                handleValueEdit(true);
            } else {
                currentRelaySetupStep = (currentRelaySetupStep + 1) % 3;
                displayServoMenu();
            }
            break;
        case 5:  // Relay timing menu (LUMI/INCAL/PUMP/FILTR)
            if (currentEditMode != NONE) {
                handleValueEdit(true);
            } else {
                if (currentEditingRelay == 3) { // FILTR
                    currentRelaySetupStep = (currentRelaySetupStep + 1) % 2;
                } else {
                    currentRelaySetupStep = (currentRelaySetupStep + 1) % 3;
                }
                displayRelayTimingMenu(currentEditingRelay);
            }
            break;
        case 6:  // REG menu - adăugăm acest caz nou
            currentRegMenu = (currentRegMenu + 1) % 3;
            regMenu();
            break;
    }
}

void MenuControl::handleCounterClockwiseRotation() {
    if (currentEditMode != NONE) {
        handleValueEdit(false);
        return;
    }
    
    // Check if we're in HOM screen, but only block if we're actually in HOM view
    if (homScreenActive && currentMenu == 0 && currentPage == 1) {
        return;
    }
    
    switch (currentPage) {
        case 0:  // Main menu
            currentMenu = (currentMenu > 0) ? currentMenu - 1 : 2;
            mainMenu();
            break;
        case 1:  // SET menu
            currentSetMenu = (currentSetMenu > 0) ? currentSetMenu - 1 : 2;
            setMenu();
            break;
        case 2:  // MAN menu
            currentManMenu = (currentManMenu > 0) ? currentManMenu - 1 : 6;
            manMenu();  // Fixed: Added parentheses and semicolon
            break;
        case 3:  // RELAY menu
            currentRelayMenu = (currentRelayMenu > 0) ? currentRelayMenu - 1 : 4;
            relayMenu();
            break;
        case 4:  // Servo menu
            currentRelaySetupStep = (currentRelaySetupStep > 0) ? currentRelaySetupStep - 1 : 2;  // Pentru rotație în sens antiorar
            displayServoMenu();
            break;
        case 5:  // Relay timing menu (LUMI/INCAL/PUMP/FILTR)
            if (currentEditMode != NONE) {
                handleValueEdit(false);
            } else {
                if (currentEditingRelay == 3) { // FILTR
                    currentRelaySetupStep = (currentRelaySetupStep > 0) ? 0 : 1;
                } else {
                    currentRelaySetupStep = (currentRelaySetupStep > 0) ? currentRelaySetupStep - 1 : 2;
                }
                displayRelayTimingMenu(currentEditingRelay);
            }
            break;
        case 6:  // REG menu - adăugăm acest caz nou
            currentRegMenu = (currentRegMenu > 0) ? currentRegMenu - 1 : 2;
            regMenu();
            break;
    }
}

void MenuControl::handleButtonPress() {
    switch (currentPage) {
        case 0:  // Main menu
            switch (currentMenu) {
                case 0: subMenuHom(); break;
                case 1: setMenu(); break;
                case 2: // Modificăm pentru a deschide meniul REG în loc de MAN
                    regMenu();
                    break;
            }
            break;
            
        case 1:  // SET menu
            switch (currentSetMenu) {
                case 0:  // RET
                    mainMenu();
                    currentPage = 0;
                    break;
                case 1:  // RELE
                    relayMenu();
                    currentPage = 3;
                    break;
                case 2:  // SERVO
                    displayServoMenu();
                    currentPage = 4;
                    break;
            }
            break;
            
        case 2:  // MAN menu
            switch (currentManMenu) {
                case 0:  // RET
                    mainMenu();
                    currentPage = 0;
                    break;
                case 1:  // LUM
                    toggleRelay(0);
                    break;
                case 2:  // INC
                    toggleRelay(1);
                    break;
                case 3:  // PUM
                    toggleRelay(2);
                    break;
                case 4:  // FIL
                    toggleRelay(3);
                    break;
                case 5:  // MIN - Servo control
                    // Show feedback
                    display.clear();
                    display.setCursor(0, 0);
                    display.print("Hranire...");
                    
                    // Start feeding sequence using saved settings
                    servoControl.setPosition(servoSettings.degrees);  // Use saved angle
                    servoFeeding = true;
                    servoStartTime = millis();
                    servoFeedDuration = servoSettings.seconds * 1000;  // Convert seconds to milliseconds
                    break;
                case 6:  // SAV - Save settings
                    saveSettings();
                    break;
            }
            break;
            
        case 3:  // RELAY menu
            switch (currentRelayMenu) {
                case 0:  // RET
                    mainMenu();
                    currentPage = 0;
                    break;
                case 1:  // LUMI
                    currentEditingRelay = 0;
                    currentPage = 5;
                    currentRelaySetupStep = 0;
                    displayRelayTimingMenu(currentEditingRelay);
                    break;
                case 2:  // INCAL
                    currentEditingRelay = 1;
                    currentPage = 5;
                    currentRelaySetupStep = 0;
                    displayRelayTimingMenu(currentEditingRelay);
                    break;
                case 3:  // PUMP
                    currentEditingRelay = 2;
                    currentPage = 5;
                    currentRelaySetupStep = 0;
                    displayRelayTimingMenu(currentEditingRelay);
                    break;
                case 4:  // HRANA
                    currentEditingRelay = 3;
                    currentPage = 5;
                    currentRelaySetupStep = 0;
                    displayRelayTimingMenu(currentEditingRelay);
                    break;
            }
            break;
            
        case 4:  // Servo menu
            switch (currentRelaySetupStep) {
                case 0:  // RET
                    setMenu();  // Revenire la meniul SET
                    currentPage = 1;
                    currentEditMode = NONE;
                    break;
                    
                case 1:  // ON (Seconds)
                    if (currentEditMode == NONE) {
                        currentEditMode = EDIT_SECONDS;
                    } else {
                        currentEditMode = NONE;
                    }
                    displayServoMenu();
                    break;
                    
                case 2:  // GRD (Degrees)
                    if (currentEditMode == NONE) {
                        currentEditMode = EDIT_DEGREES;
                    } else {
                        currentEditMode = NONE;
                    }
                    displayServoMenu();
                    break;
            }
            break;
            
        case 5:  // Relay timing menu
            switch (currentRelaySetupStep) {
                case 0:  // RET
                    relayMenu();  // Întoarcere la meniul RELE
                    currentPage = 3;
                    currentEditMode = NONE;
                    break;
                    
                case 1:  // ON/OFF time depending on relay type
                    if (currentEditingRelay == 3) {  // FILTR - gestionare specială pentru OFF
                        if (currentEditMode == NONE) {
                            currentEditMode = EDIT_MINUTES;
                        } else {
                            currentEditMode = NONE;
                        }
                        displayRelayTimingMenu(currentEditingRelay);
                    } else {  // Celelalte relee - normal ON
                        if (currentEditMode == NONE) {
                            startEditing(currentRelaySetupStep);
                        } else {
                            if (currentEditingRelay == 0) {  // Pentru LUMI
                                if (currentEditMode == EDIT_HOURS) {
                                    currentEditMode = EDIT_MINUTES;
                                } else {
                                    currentEditMode = NONE;
                                }
                            } else {  // Pentru alte relee
                                currentEditMode = NONE;
                            }
                        }
                        displayRelayTimingMenu(currentEditingRelay);
                    }
                    break;
                    
                case 2:  // OFF time (doar pentru relee, nu pentru FILTR)
                    if (currentEditMode == NONE) {
                        startEditing(currentRelaySetupStep);
                    } else {
                        if (currentEditingRelay == 0) {  // Pentru LUMI
                            if (currentEditMode == EDIT_HOURS) {
                                currentEditMode = EDIT_MINUTES;
                            } else {
                                currentEditMode = NONE;
                            }
                        } else {  // Pentru alte relee
                            currentEditMode = NONE;
                        }
                    }
                    displayRelayTimingMenu(currentEditingRelay);
                    break;
            }
            break;
            
        case 6:  // REG menu - adăugăm acest caz nou
            switch (currentRegMenu) {
                case 0:  // RET
                    mainMenu();
                    currentPage = 0;
                    break;
                case 1:  // AUTO
                    // Activare regim automat
                    autoMode = true;
                    regMenu();  // Reafișăm meniul pentru a actualiza indicatorii
                    
                    // Activăm toate releele pentru control automat
                    for (int i = 0; i < 4; i++) {
                        relayTimings[i].enabled = true;
                    }
                    
                    // Verificăm și actualizăm imediat starea releelor
                    verificaStareRelee();
                    
                    // Feedback pentru utilizator
                    display.clear();
                    display.setCursor(0, 0);
                    display.print("Regim AUTO");
                    display.setCursor(0, 1);
                    display.print("activat!");
                    
                    showingRelayFeedback = true;
                    relayFeedbackTime = millis();
                    break;
                case 2:  // MAN
                    // Activare regim manual
                    autoMode = false;
                    
                    // Dezactivăm toate releele pentru control automat
                    for (int i = 0; i < 4; i++) {
                        relayTimings[i].enabled = false;
                    }
                    
                    // Intrăm direct în meniul MAN
                    manMenu();
                    currentPage = 2;
                    break;
            }
            break;
    }
}

void MenuControl::displayServoMenu() {
    display.clear();
    display.setCursor(0, 0);
    display.print("*SERVO");
    
    // Always display RET at position 1,1
    display.setCursor(1, 1);
    display.print("RET");
    
    // Show marker for current selection
    switch (currentRelaySetupStep) {
        case 0:  // RET selected
            display.setCursor(0, 1);
            display.print(">");
            break;
        case 1:  // ON selected
            display.setCursor(6, 0);
            display.print(">");
            break;
        case 2:  // GRD selected
            display.setCursor(5, 1);
            display.print(">");
            break;
    }
    
    // Display ON section with seconds
    display.setCursor(7, 0);
    display.print("ON");
    display.setCursor(11, 0);
    char timeStr[4];
    snprintf(timeStr, sizeof(timeStr), "%02uS", servoSettings.seconds);
    display.print(timeStr);
    
    if (currentEditMode == EDIT_SECONDS && currentRelaySetupStep == 1) {
        display.setCursor(10, 0);
        display.print("*");
    }
    
    // Display GRD section with degrees
    display.setCursor(6, 1);
    display.print("GRD");
    display.setCursor(11, 1);
    char degStr[5];
    snprintf(degStr, sizeof(degStr), "%03u\xDF", servoSettings.degrees);
    display.print(degStr);
    
    if (currentEditMode == EDIT_DEGREES && currentRelaySetupStep == 2) {
        display.setCursor(10, 1);
        display.print("*");
    }
}

void MenuControl::toggleRelay(int relayNum) {
    if (relayNum >= 0 && relayNum < 4) {
        bool currentState = relayControl.getState(relayNum);
        relayControl.setState(relayNum, !currentState);
        
        // Show feedback
        display.clear();
        display.setCursor(0, 0);
        const char* relayNames[] = {"Lumina", "Incalzire", "Pompa", "Filtr"};
        display.print(relayNames[relayNum]);
        display.print(": ");
        display.print(!currentState ? "ON" : "OFF");
        
        showingRelayFeedback = true;
        relayFeedbackTime = millis();
    }
}

void MenuControl::displayRelayTimingMenu(int relayIndex) {
    const char* relayNames[] = {"LUMI", "INCAL", "PUMP", "FILTR"};
    
    display.clear();
    display.setCursor(0, 0);
    display.print("*");
    display.print(relayNames[relayIndex]);
    
    // Întotdeauna afișează RET pe prima poziție
    display.setCursor(1, 1);
    display.print("RET");
    
    // Adaugă marker pentru poziția curentă selectată
    if (currentRelaySetupStep == 0) {
        display.setCursor(0, 1);
        display.print(">");  // Marker pentru RET
    }

    // Tratare specială pentru FILTR (relayIndex == 3)
    if (relayIndex == 3) {  // FILTR
        // Pentru FILTR, afișăm doar OFF care reprezintă minutele de oprire
        // Punem OFF pe linia a doua, la fel ca celelalte meniuri
        display.setCursor(6, 1);
        display.print("OFF");
        display.setCursor(11, 1);
        
        // Valoarea în minute pentru oprirea filtrului
        char timeStr[7];
        snprintf(timeStr, sizeof(timeStr), "%02umin", relayTimings[relayIndex].offMinute);
        display.print(timeStr);
        
        if (currentEditMode != NONE && currentRelaySetupStep == 1) {
            display.setCursor(10, 1);
            display.print("*");
        }
        
        // Adăugăm marker pentru OFF dacă este selectat
        if (currentRelaySetupStep == 1) {
            display.setCursor(5, 1);
            display.print(">");  // Marker pentru OFF
        }
    }
    else if (relayIndex == 1) {  // INCAL - nu modificăm codul existent
        display.setCursor(7, 0);
        display.print("ON");
        display.setCursor(10, 0);
        display.print(currentEditMode == EDIT_TEMP && currentRelaySetupStep == 1 ? "*" : " ");
        char tempStr[6];
        snprintf(tempStr, sizeof(tempStr), "%02u\xDF""C", 
                relayTimings[relayIndex].onHour);
        display.print(tempStr);
        
        display.setCursor(7, 1);
        display.print("OFF");
        display.setCursor(10, 1);
        display.print(currentEditMode == EDIT_TEMP && currentRelaySetupStep == 2 ? "*" : " ");
        snprintf(tempStr, sizeof(tempStr), "%02u\xDF""C", 
                relayTimings[relayIndex].offHour);
        display.print(tempStr);
        
        // Markerele pentru ON și OFF
        if (currentRelaySetupStep == 1) {
            display.setCursor(6, 0);
            display.print(">");  // Marker pentru ON
        } else if (currentRelaySetupStep == 2) {
            display.setCursor(6, 1);
            display.print(">");  // Marker pentru OFF
        }
    } 
    else if (relayIndex == 2) {  // PUMP
        // ON section
        display.setCursor(7, 0);
        display.print("ON");
        display.setCursor(11, 0);
        display.print(currentEditMode == EDIT_LEVEL && currentRelaySetupStep == 1 ? "*" : " ");
        char levelStr[7];
        snprintf(levelStr, sizeof(levelStr), "%03umm", 
                relayTimings[relayIndex].onHour);
        display.print(levelStr);
        
        // OFF section
        display.setCursor(6, 1);
        display.print("OFF");
        display.setCursor(11, 1);
        display.print(currentEditMode == EDIT_LEVEL && currentRelaySetupStep == 2 ? "*" : " ");
        snprintf(levelStr, sizeof(levelStr), "%03umm", 
                relayTimings[relayIndex].offHour);
        display.print(levelStr);
        
        // Markerele pentru ON și OFF
        if (currentRelaySetupStep == 1) {
            display.setCursor(6, 0);
            display.print(">");  // Marker pentru ON
        } else if (currentRelaySetupStep == 2) {
            display.setCursor(5, 1);
            display.print(">");  // Marker pentru OFF
        }
    }
    else {  // LUMI (relayIndex == 0)
        display.setCursor(7, 0);
        display.print("ON");
        display.setCursor(11, 0);  // Start time at position 11
        
        // ON time display and editing
        char timeStr[6];
        snprintf(timeStr, sizeof(timeStr), "%02u:%02u", 
                relayTimings[relayIndex].onHour,
                relayTimings[relayIndex].onMinute);
        display.print(timeStr);
        
        if (currentEditMode != NONE && currentRelaySetupStep == 1) {
            display.setCursor(currentEditMode == EDIT_HOURS ? 10 : 13, 0);
            display.print("*");
        }
        
        // OFF time display
        display.setCursor(6, 1);
        display.print("OFF");
        display.setCursor(11, 1);
        
        // OFF time display and editing
        snprintf(timeStr, sizeof(timeStr), "%02u:%02u",
                relayTimings[relayIndex].offHour,
                relayTimings[relayIndex].offMinute);
        display.print(timeStr);
        
        if (currentEditMode != NONE && currentRelaySetupStep == 2) {
            display.setCursor(currentEditMode == EDIT_HOURS ? 10 : 13, 1);
            display.print("*");
        }
        
        // Markerele pentru ON și OFF
        if (currentRelaySetupStep == 1) {
            display.setCursor(6, 0);
            display.print(">");  // Marker pentru ON
        } else if (currentRelaySetupStep == 2) {
            display.setCursor(5, 1);
            display.print(">");  // Marker pentru OFF
        }
    }
}

void MenuControl::startEditing(int step) {
    isEditingOn = (step == 1);
    
    switch (currentEditingRelay) {
        case 0:  // LUMI
            currentEditMode = EDIT_HOURS;
            break;
        case 1:  // INCAL
            currentEditMode = EDIT_TEMP;
            break;
        case 2:  // PUMP
            currentEditMode = EDIT_LEVEL;
            break;
        case 3:  // FILTR - modificat pentru a edita doar minutele
            currentEditMode = EDIT_MINUTES;
            break;
    }
}

void MenuControl::handleValueEdit(bool increase) {
    int increment = increase ? 1 : -1;
    
    // Verifică mai întâi dacă suntem în meniul SERVO
    if (currentPage == 4) {
        switch (currentEditMode) {
            case EDIT_SECONDS:
                servoSettings.seconds = constrain(servoSettings.seconds + increment, 1, 10);
                displayServoMenu();
                return;  // Important: ieșim din funcție după afișare
                
            case EDIT_DEGREES:
                servoSettings.degrees = constrain(servoSettings.degrees + increment * 5, 0, 180);
                displayServoMenu();
                return;  // Important: ieșim din funcție după afișare
        }
    }
    
    // Procesăm editarea pentru celelalte meniuri
    RelayTiming& timing = relayTimings[currentEditingRelay];
    
    switch (currentEditMode) {
        case EDIT_HOURS:  // Pentru LUMI - ore
            if (isEditingOn) {
                timing.onHour = (timing.onHour + increment + 24) % 24;
            } else {
                timing.offHour = (timing.offHour + increment + 24) % 24;
            }
            break;
            
        case EDIT_MINUTES:
            if (currentEditingRelay == 3) {  // Special case for FILTR
                // Limităm la valori între 1-10 minute pentru filtru
                timing.offMinute = constrain(timing.offMinute + increment, 1, 10);
            } else {
                if (isEditingOn) {
                    timing.onMinute = (timing.onMinute + increment + 60) % 60;
                } else {
                    timing.offMinute = (timing.offMinute + increment + 60) % 60;
                }
            }
            break;
            
        case EDIT_TEMP:
            if (isEditingOn) {
                timing.onHour = constrain(timing.onHour + increment, 15, 35);  // Temperature range: 15-35°C
            } else {
                timing.offHour = constrain(timing.offHour + increment, 15, 35);
            }
            break;
            
        case EDIT_LEVEL:
            int step = 5;  // Change water level by 5mm increments
            if (isEditingOn) {
                timing.onHour = constrain(timing.onHour + (increment * step), 50, 300);  // Level range: 50-300mm
            } else {
                timing.offHour = constrain(timing.offHour + (increment * step), 50, 300);
            }
            break;
    }
    
    displayRelayTimingMenu(currentEditingRelay);
}

// Modificarea metodei update() pentru a asigura verificarea în toate meniurile

void MenuControl::update() {
    unsigned long currentTime = millis();
    
    // PRIORITATE 1: Întotdeauna verifică și actualizează starea releelor dacă suntem în regim auto
    // Aceasta este PRIMA operație din update pentru a asigura că are prioritate
    static unsigned long lastRelayCheckTime = 0;
    if (currentTime - lastRelayCheckTime >= 1000) {  // La fiecare secundă
        if (autoMode) {
            // Afișăm starea curentă pentru debug
            Serial.println("Verificare relee... (regim AUTO)");
            
            // Verifică TOATE releele, inclusiv LUMI
            verificaStareRelee();
        }
        lastRelayCheckTime = currentTime;
    }
    
    // Restul operațiunilor după verificarea releelor
    
    // Gestionarea servo-ului pentru hrănire
    if (servoFeeding) {
        if (currentTime - servoStartTime >= servoFeedDuration) {
            servoControl.setPosition(0);
            servoFeeding = false;
            
            display.clear();
            display.setCursor(0, 0);
            display.print("Hranire completa");
            
            showingRelayFeedback = true;
            relayFeedbackTime = currentTime;
        }
    }
    
    // Actualizăm ecranul HOME dacă este activ
    updateHomScreen();
    
    // Verificăm timeout-urile pentru meniuri DUPĂ verificarea releelor
    checkMenuSelectionTimeout(currentTime);
}

// Reimplementare completă a verificaStareRelee() pentru a asigura funcționarea în toate meniurile

void MenuControl::verificaStareRelee() {
    // Dacă nu suntem în regim automat, ieșim
    if (!autoMode) return;
    
    // Obține ora și data curentă
    String timeString = rtcControl.getTime(); // Format: "HH:MM"
    
    // Verifică formatul
    if (timeString.length() < 5) {
        Serial.println("ERROR: Format timp invalid");
        return;
    }
    
    // Parsează ora și minutul
    int currentHour = timeString.substring(0, 2).toInt();
    int currentMinute = timeString.substring(3, 5).toInt();
    int currentTimeInMinutes = currentHour * 60 + currentMinute;
    
    // Debug pentru timpul curent
    Serial.print("Timp curent: ");
    Serial.print(currentHour);
    Serial.print(":");
    Serial.println(currentMinute);
    
    // Verifică FIECARE releu, începând de la 0 (LUMI)
    for (int i = 0; i < 4; i++) {
        // Dacă releul nu este activat pentru control automat, continuă
        if (!relayTimings[i].enabled) continue;
        
        bool shouldBeOn = false;
        
        // Cazul special pentru PUMP (index 2) - bazat pe nivel apă
        if (i == 2) {
            int currentWaterLevel = ultrasonicControl.getDistance();
            
            if (currentWaterLevel <= relayTimings[i].onHour) {
                shouldBeOn = true;
            } 
            else if (currentWaterLevel >= relayTimings[i].offHour) {
                shouldBeOn = false;
            }
            else {
                shouldBeOn = relayControl.getState(i);
            }
            
            // Debug pentru PUMP
            Serial.print("PUMP: nivel=");
            Serial.print(currentWaterLevel);
            Serial.print(", ON<=");
            Serial.print(relayTimings[i].onHour);
            Serial.print(", OFF>=");
            Serial.print(relayTimings[i].offHour);
            Serial.print(", stare=");
            Serial.println(shouldBeOn ? "ON" : "OFF");
        } 
        else {
            // Pentru LUMI (0), INCAL (1), FILTR (3) - bazat pe timp
            int onTimeInMinutes = relayTimings[i].onHour * 60 + relayTimings[i].onMinute;
            int offTimeInMinutes = relayTimings[i].offHour * 60 + relayTimings[i].offMinute;
            
            if (onTimeInMinutes < offTimeInMinutes) {
                // Pornește și se oprește în aceeași zi
                shouldBeOn = (currentTimeInMinutes >= onTimeInMinutes && 
                             currentTimeInMinutes < offTimeInMinutes);
            } else {
                // Pornește într-o zi și se oprește în următoarea
                shouldBeOn = (currentTimeInMinutes >= onTimeInMinutes || 
                             currentTimeInMinutes < offTimeInMinutes);
            }
            
            // Debug special pentru LUMI
            if (i == 0) {
                Serial.print("LUMI: ");
                Serial.print(currentTimeInMinutes);
                Serial.print(" minute, ON=");
                Serial.print(onTimeInMinutes);
                Serial.print(", OFF=");
                Serial.print(offTimeInMinutes);
                Serial.print(", stare=");
                Serial.println(shouldBeOn ? "ON" : "OFF");
            }
        }
        
        // Aici este partea crucială - actualizăm starea releului direct, indiferent de meniu
        bool currentState = relayControl.getState(i);
        if (shouldBeOn != currentState) {
            // Debug pentru schimbarea de stare
            Serial.print("Releu ");
            Serial.print(i);
            Serial.print(" (");
            if (i == 0) Serial.print("LUMI");
            else if (i == 1) Serial.print("INCAL");
            else if (i == 2) Serial.print("PUMP");
            else if (i == 3) Serial.print("FILTR");
            Serial.print("): ");
            Serial.print(currentState ? "ON" : "OFF");
            Serial.print(" -> ");
            Serial.println(shouldBeOn ? "ON" : "OFF");
            
            // IMPORTANT: Actualizăm starea direct la nivel hardware
            relayControl.setState(i, shouldBeOn);
        }
    }
}

// Adăugați această implementare înainte de verificaStareRelee()

void MenuControl::verificaLUMI() {
    // Dacă nu suntem în regim automat sau releul nu este activat, ieșim
    if (!autoMode || !relayTimings[0].enabled) return;
    
    // Obține ora și data curentă
    String timeString = rtcControl.getTime(); // Format: "HH:MM"
    
    // Verifică formatul
    if (timeString.length() < 5) return;
    
    // Parsează ora și minutul
    int currentHour = timeString.substring(0, 2).toInt();
    int currentMinute = timeString.substring(3, 5).toInt();
    int currentTimeInMinutes = currentHour * 60 + currentMinute;
    
    // Calculăm minutele pentru pornire/oprire
    int onTimeInMinutes = relayTimings[0].onHour * 60 + relayTimings[0].onMinute;
    int offTimeInMinutes = relayTimings[0].offHour * 60 + relayTimings[0].offMinute;
    
    // Determinăm dacă lumina ar trebui să fie pornită
    bool shouldBeOn = false;
    
    if (onTimeInMinutes < offTimeInMinutes) {
        // Pornește și se oprește în aceeași zi
        shouldBeOn = (currentTimeInMinutes >= onTimeInMinutes && 
                     currentTimeInMinutes < offTimeInMinutes);
    } else {
        // Pornește într-o zi și se oprește în următoarea
        shouldBeOn = (currentTimeInMinutes >= onTimeInMinutes || 
                     currentTimeInMinutes < offTimeInMinutes);
    }
    
    // Actualizarea stării releului LUMI
    bool currentState = relayControl.getState(0);
    if (shouldBeOn != currentState) {
        relayControl.setState(0, shouldBeOn);
    }
}

// Modificarea funcției saveSettings() pentru a include actualizarea stării releelor după salvare
void MenuControl::saveSettings() {
    int eepromAddress = 0;
    const uint8_t MAGIC_NUMBER = 42;
    
    // Write magic number to verify data is valid when reading
    EEPROM.update(eepromAddress++, MAGIC_NUMBER);
    
    // Save relay timing settings
    for (int i = 0; i < 4; i++) {
        EEPROM.update(eepromAddress++, relayTimings[i].onHour);
        EEPROM.update(eepromAddress++, relayTimings[i].onMinute);
        EEPROM.update(eepromAddress++, relayTimings[i].offHour);
        EEPROM.update(eepromAddress++, relayTimings[i].offMinute);
        EEPROM.update(eepromAddress++, relayTimings[i].enabled ? 1 : 0);
    }
    
    // Save servo settings
    EEPROM.update(eepromAddress++, servoSettings.seconds);
    EEPROM.update(eepromAddress++, servoSettings.degrees);
    
    // Salvăm și autoMode
    EEPROM.update(eepromAddress++, autoMode ? 1 : 0);
    
    // Show confirmation message
    display.clear();
    display.setCursor(0, 0);
    display.print("Setari salvate");
    display.setCursor(0, 1);
    display.print("cu succes!");
    
    // Set feedback state
    showingRelayFeedback = true;
    relayFeedbackTime = millis();
    
    // După salvare, actualizăm imediat starea releelor dacă suntem în modul automat
    if (autoMode) {
        // Asigurăm că toate releele sunt activate pentru controlul automat
        for (int i = 0; i < 4; i++) {
            relayTimings[i].enabled = true;
        }
        
        // Actualizăm starea tuturor releelor (inclusiv LUMI)
        verificaStareRelee();
    }
}

// Add this function to load settings from EEPROM
void MenuControl::loadSettings() {
    int eepromAddress = 0;
    const uint8_t MAGIC_NUMBER = 42;
    
    // Check if EEPROM has been initialized with our settings
    uint8_t magic = EEPROM.read(eepromAddress++);
    if (magic != MAGIC_NUMBER) {
        // EEPROM has not been initialized, use default values
        // Pentru filtru setăm implicit 3 minute
        relayTimings[3].offMinute = 3;
        
        // Asigurăm regimul AUTO la prima pornire
        autoMode = true;
        
        // Activăm toate releele pentru control automat
        for (int i = 0; i < 4; i++) {
            relayTimings[i].enabled = true;
        }
        
        return;
    }
    
    // Load relay timing settings
    for (int i = 0; i < 4; i++) {
        relayTimings[i].onHour = EEPROM.read(eepromAddress++);
        relayTimings[i].onMinute = EEPROM.read(eepromAddress++);
        relayTimings[i].offHour = EEPROM.read(eepromAddress++);
        relayTimings[i].offMinute = EEPROM.read(eepromAddress++);
        relayTimings[i].enabled = EEPROM.read(eepromAddress++) == 1;
    }
    
    // Load servo settings
    servoSettings.seconds = EEPROM.read(eepromAddress++);
    servoSettings.degrees = EEPROM.read(eepromAddress++);
    
    // Încărcăm autoMode dacă există în EEPROM
    if (eepromAddress < EEPROM.length()) {
        // Citim valoarea din EEPROM dar o ignorăm pentru a forța regimul AUTO la pornire
        // autoMode = EEPROM.read(eepromAddress++) == 1;
        eepromAddress++; // Doar incrementăm adresa pentru a menține structura corectă
    }
    
    // Forțăm regimul AUTO la pornire indiferent de valoarea salvată
    autoMode = true;
    
    // Activăm toate releele pentru control automat
    for (int i = 0; i < 4; i++) {
        relayTimings[i].enabled = true;
    }
    
    // Debug pentru a verifica setarea corectă
    Serial.println("Pornire sistem: Regim AUTO activat");
}

// Adăugare în constructor sau în metoda de inițializare
// Asigurați-vă că această metodă este apelată la pornirea sistemului
void MenuControl::initializeRelays() {
    // Forțăm regimul AUTO la pornire
    autoMode = true;
    
    // Activăm toate releele pentru control automat
    for (int i = 0; i < 4; i++) {
        relayTimings[i].enabled = true;
    }
    
    // Verifică starea releelor imediat la inițializare
    verificaStareRelee();
    
    Serial.println("Relee inițializate în regim AUTO");
}

// Implementarea metodei publice pentru a verifica starea releelor
void MenuControl::checkRelayStates() {
    // Forțăm verificarea stării releelor
    verificaStareRelee();
}
