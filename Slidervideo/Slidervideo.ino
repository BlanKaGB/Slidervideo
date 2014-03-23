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
#include "MenuLCD.h"

typedef enum {
    MoteurStatutArret,
    MoteurStatutAvant,
    MoteurStatutArriere,
} MoteurStatut;

#define FDC_HOME_PIN    13
#define FDC_END_PIN     12
#define FDC_ACTIVE      HIGH
#define MAX_PAS         0xFFFFFFFF

SnootorStep Moteur1;
uint32_t pasMoteur = 400;
uint32_t parMoteurDelta = 1000;
MenuLCD menuLCD;
unsigned long moteurStartTime = 0;
uint32_t pasMoteurStart = 0;

MoteurStatut moteurStatut = MoteurStatutArret;

void setup()
{
    Serial.begin(115200);
    Wire.begin(); // join i2c
  
    Moteur1.init(2000,200,1,MOTOR_MODE_FULLSTEP); // moteur 200 pas/tour
    
    pinMode(FDC_HOME_PIN, INPUT_PULLUP); // declare la pin digital FDC_HOME_PIN en entree
    pinMode(FDC_END_PIN, INPUT_PULLUP); // declare la pin digital FDC_END_PIN en entree
    
    menuLCD.init();
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
            moteurStatut = MoteurStatutAvant;
            if (pas == MAX_PAS) {
                sprintf(buffer, "Avance : MAX");
            } else {
                sprintf(buffer, "Avance : %u       ", pas);
            }
        } else {
            Moteur1.forward(pas);
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

void loop()
{       
    char buffer[32];
    
    if (digitalRead(FDC_HOME_PIN) == FDC_ACTIVE && moteurStatut == MoteurStatutArriere) {
        // Si on revient en arriere et que le capteur de debut s'active, on arrete
        Serial.println("Stop: FDC Home");
        menuLCD.clear();
        menuLCD.displayMessage("Debut de course", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    }
    if (digitalRead(FDC_END_PIN) == FDC_ACTIVE && moteurStatut == MoteurStatutAvant) {
        // Si on part en avant et que le capteur de fin s'active, on arrete
        Serial.println("Stop: FDC End");
        menuLCD.clear();
        menuLCD.displayMessage("Fin de course", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    }
    
    switch(menuLCD.getKey()) {
    
    // Right: Arriere
    case 0:
        deplaceMoteur(pasMoteur, false);
        break; 

    // Left: Avant
    case 3: // Direction 2 en manuel
        deplaceMoteur(pasMoteur, true);
        break;
            
    // Up: Pas +
    case 1:
        pasMoteur += parMoteurDelta;
        sprintf(buffer, "Pas + : %d", pasMoteur);
        menuLCD.displayMessage(buffer, NULL, 1000);
        break;
              
    // Down: Pas -
    case 2:
        pasMoteur -= parMoteurDelta;
        if (pasMoteur < parMoteurDelta) {
            pasMoteur = parMoteurDelta;
        }
        sprintf(buffer, "Pas - : %d", pasMoteur);
        menuLCD.displayMessage(buffer, NULL, 1000);
        break;

    // Select: Arret
    case 4: // Stop
        if (moteurStatut != MoteurStatutArret) {
            menuLCD.clear();
            menuLCD.displayMessage("Arret moteur", NULL, 1000);
            Moteur1.stop();
            moteurStatut = MoteurStatutArret;
        }
        break;


    default:
        break;
    }
    if (!Moteur1.stopped()) {
        if (moteurStartTime == 0) {
            moteurStartTime = millis();
        }
        SC.delay(200);
        if (Moteur1.stopped()) {
            menuLCD.clear();
            Serial.print("Moteur : ");
            Serial.println(millis() - moteurStartTime);
            Serial.print("Temps par pas : ");
            Serial.println((millis() - moteurStartTime) / (double)pasMoteurStart);
            moteurStart = 0;
        }
    }
    menuLCD.loop();
}

