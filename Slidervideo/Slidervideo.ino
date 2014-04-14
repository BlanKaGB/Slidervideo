/*
 * Snootlab Deuligne joystick et Motorshield
 * Initial code from nuelectronics
 * 
 * copyleft 2011 snootlab
 * free software, have fun !
 *
 * 1 tour = 400 pas = 47,5 mm
 * 1 pas = 0,11865 mm = 0,012 cm
 *
 * A modifier:
 *	case 4 devrait arreter le moteur en cours de mouvement 
 *	afficher le deplacement en CM ou MM sur le LCD
 *	Ajouter un switch et une fonction “initialisation” le moteur cherche la butée en debut de programme.
 *
 */

#include <Wire.h> // I2C library include
#include <Deuligne.h> // LCD library include
#include <snootor.h>
#include <EEPROM.h>
#include "MenuLCD.h"
#include "Sauvegarde.h"

typedef enum {
    MoteurStatutArret,
    MoteurStatutAvant,
    MoteurStatutArriere,
} MoteurStatut;

#define FDC_HOME_PIN    13            // déclaration input switch home
#define FDC_END_PIN     12            // déclaration input switch fin de course
#define FDC_ACTIVE      HIGH          // on declare les fins de course NF
#define MAX_PAS         0x7FFFFFFF    // on declare la valeur MAX de pas (utilisé pour nombre de pas maximun)
#define MAX_VITESSE     3000          // Changer la valeur MAX_VITESSE (par la vitesse maxi du moteur) 
#define MIN_VITESSE     600          // MIN_VITESSE

#define MENU_AVANCER                  1
#define MENU_RECULER                  2
#define MENU_DEBUT                    3
#define MENU_FIN                      4
#define MENU_CHANGER_PAS              5
#define MENU_CHANGER_PAS_DELTA        6
#define MENU_CHANGER_VITESSE          7
#define MENU_CHANGER_VITESSE_DELTA    8

Sauvegarde sauvegarde;
SnootorStep Moteur1;
uint32_t pasMoteur = 400;
uint32_t pasMoteurDelta = 1000;
uint32_t vitesse = 50;
uint32_t vitesseDelta = 1000;
MenuLCD menuLCD;
unsigned long moteurStartTime = 0;
uint32_t pasMoteurStart = 0;

MoteurStatut moteurStatut = MoteurStatutArret;

void setup()
{
    MenuLCDMenuItem *mainMenu;
    MenuLCDMenuItem *menuItem;
    
    Serial.begin(115200);
    Wire.begin(); // join i2c
  
    if (sauvegarde.load()) {
        Serial.println("EEPROM valide");
    } else {
        Serial.println("EEPROM invalide");
    }
    pasMoteur = sauvegarde.pasMoteur();
    pasMoteurDelta = sauvegarde.pasMoteurDelta();
    vitesse = sauvegarde.vitesse();
    vitesseDelta = sauvegarde.vitesseDelta();
    
    Moteur1.init(1000,200,1,MOTOR_MODE_HALFSTEP); // moteur 200 pas/tour
    
    
    pinMode(FDC_HOME_PIN, INPUT_PULLUP); // declare la pin digital FDC_HOME_PIN en entree
    pinMode(FDC_END_PIN, INPUT_PULLUP); // declare la pin digital FDC_END_PIN en entree
    
    menuLCD.init();
    
    mainMenu = menuLCD.getMainMenuItem();
    menuLCD.addSubMenuItem(mainMenu, "AVANCER", MENU_AVANCER);
    menuLCD.addSubMenuItem(mainMenu, "RECULER", MENU_RECULER);
    menuLCD.addSubMenuItem(mainMenu, "DEBUT", MENU_DEBUT);
    menuLCD.addSubMenuItem(mainMenu, "FIN", MENU_FIN);
    menuItem = menuLCD.addSubMenuItem(mainMenu, "PARAMETRES", 0);
    menuLCD.addSubMenuItem(menuItem, "CHANGE PAS...", MENU_CHANGER_PAS);
    menuLCD.addSubMenuItem(menuItem, "CHANGE DELTA...", MENU_CHANGER_PAS_DELTA);
    menuLCD.addSubMenuItem(menuItem, "VITESSE ...", MENU_CHANGER_VITESSE);
    menuLCD.addSubMenuItem(menuItem, "VITESSE DELTA...", MENU_CHANGER_VITESSE_DELTA);

    Serial.print(FDC_HOME_PIN);
    Serial.print(" " );
    Serial.println((digitalRead(FDC_HOME_PIN) == LOW)?"LOW":"HIGH");
    Serial.print(FDC_END_PIN);
    Serial.print(" " );
    Serial.println((digitalRead(FDC_END_PIN) == LOW)?"LOW":"HIGH");
    
    // retour en arriere avec le nombre de pas max possible
    // pour atteindre le debut de course
    deplaceMoteur(MAX_PAS, false);
}

void deplaceMoteur(uint32_t pas, boolean avance)
{
    char buffer[32];

    if (avance && digitalRead(FDC_END_PIN) == FDC_ACTIVE) {
        menuLCD.displayMessage("Fin de course", NULL, 1000);
        Serial.print("Fin de course : butee ");
        Serial.println(FDC_END_PIN);
    } else if (!avance && digitalRead(FDC_HOME_PIN) == FDC_ACTIVE) {
        menuLCD.displayMessage("Debut de course", NULL, 1000);
        Serial.print("Debut de course : butee ");
        Serial.println(FDC_HOME_PIN);
    } else {
        if (avance) {
            Moteur1.back(pas);
            Moteur1.setDelay(vitesse); 
            moteurStatut = MoteurStatutAvant;
            if (pas == MAX_PAS) {
                sprintf(buffer, "Avance : MAX");
            } else {
                sprintf(buffer, "Avance : %u       ", pas);
            }
        } else {
            Moteur1.forward(pas);
            Moteur1.setDelay(vitesse);
            moteurStatut = MoteurStatutArriere;
            if (pas == MAX_PAS) {
                sprintf(buffer, "Arriere : MAX");
            } else {
                sprintf(buffer, "Arriere : %u       ", pas);
            }
        }
        menuLCD.displayMessage(buffer, NULL);
        Serial.print("Pas moteur : ");
        Serial.println(pas);
        pasMoteurStart = pasMoteur;
    }
}

void changePas(long value)
{
    pasMoteur = value;
    sauvegarde.setPasMoteur(pasMoteur);
    sauvegarde.save();
}

void changePasDelta(long value)
{
    pasMoteurDelta = value;
    sauvegarde.setPasMoteurDelta(pasMoteurDelta);
    sauvegarde.save();
}

void changevitesse(long value)
{
    vitesse = value;
    sauvegarde.setvitesse(vitesse);
    sauvegarde.save();
    Serial.print("Vitesse :   ");
    Serial.println(vitesse);
}

void changevitesseDelta(long value)
{
    vitesseDelta = value;
    sauvegarde.setvitesseDelta(vitesseDelta);
    sauvegarde.save();
}

void loop()
{       
    char buffer[32];
    
    if (moteurStatut == MoteurStatutArret) {
        int identifier;
        
        switch(menuLCD.selectedMenuIdentifer()) {
        case MENU_AVANCER:
            deplaceMoteur(pasMoteur, true);
            break;
        case MENU_RECULER:
            deplaceMoteur(pasMoteur, false);
            break;
        case MENU_DEBUT:
            deplaceMoteur(MAX_PAS, false);
            break;
        case MENU_FIN:
            deplaceMoteur(MAX_PAS, true);
            break;
        case MENU_CHANGER_PAS:
            menuLCD.editValue(pasMoteur, pasMoteurDelta, pasMoteurDelta, MAX_PAS, "Pas :", changePas);
            break;
        case MENU_CHANGER_PAS_DELTA:
            menuLCD.editValue(pasMoteurDelta, 100, 100, MAX_PAS, "Pas delta :", changePasDelta);
            break;
        case MENU_CHANGER_VITESSE:
            menuLCD.editValue(vitesse, vitesseDelta, MIN_VITESSE, MAX_VITESSE, "Vitesse :", changevitesse);
            break;
        case MENU_CHANGER_VITESSE_DELTA:
            menuLCD.editValue(vitesseDelta, 50, 50, MAX_VITESSE, "Vitesse delta :", changevitesseDelta);
            break;
        }
    } else if (digitalRead(FDC_HOME_PIN) == FDC_ACTIVE && moteurStatut == MoteurStatutArriere) {
        // Si on revient en arriere et que le capteur de debut s'active, on arrete
        Serial.println("Stop: FDC Home");
        menuLCD.clear();
        menuLCD.displayMessage("Debut de course", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    } else if (digitalRead(FDC_END_PIN) == FDC_ACTIVE && moteurStatut == MoteurStatutAvant) {
        // Si on part en avant et que le capteur de fin s'active, on arrete
        Serial.println("Stop: FDC End");
        menuLCD.clear();
        menuLCD.displayMessage("Fin de course", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    } else if (menuLCD.getKey(NULL) == 4) {
        Serial.println("Stop");
        menuLCD.clear();
        menuLCD.displayMessage("Stop", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    } else {
        if (moteurStartTime == 0) {
            moteurStartTime = millis();
        }
        SC.delay(200);
        if (Moteur1.stopped()) {
            moteurStatut = MoteurStatutArret;
            menuLCD.clear();
            Serial.print("Moteur : ");
            Serial.println(millis() - moteurStartTime);
            Serial.print("Temps par pas : ");
            Serial.println((millis() - moteurStartTime) / (double)pasMoteurStart);
            moteurStartTime = 0;
        }
    }
    menuLCD.loop();
}

