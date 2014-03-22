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

SnootorStep Moteur1;
int pasMoteur = 400;
int parMoteurDelta = 1000;
MenuLCD menuLCD;
unsigned int moteurStart = 0;
unsigned int pasMoteurStart = 0;

MoteurStatut moteurStatut = MoteurStatutArret;

void setup()
{
    Serial.begin(115200);
    Wire.begin(); // join i2c
  
    Moteur1.init(2000,200,1,MOTOR_MODE_FULLSTEP); // moteur 200 pas/tour
    
    pinMode(FDC_HOME_PIN,INPUT_PULLUP); // declare la pin digital FDC_HOME_PIN en entree
    pinMode(FDC_END_PIN,INPUT_PULLUP); // declare la pin digital FDC_END_PIN en entree
    
    menuLCD.init();
    menuLCD.displayMessage("Pret...", NULL, 2000);
}

void deplaceMoteur(int pas, boolean avance)
{
    char buffer[32];

    if (avance) {
        Moteur1.forward(pas);
        moteurStatut = MoteurStatutAvant;
        sprintf(buffer, "Arriere : %d       ", pas);
    } else {
        Moteur1.back(pas);
        moteurStatut = MoteurStatutArriere;
        sprintf(buffer, "Avance : %d       ", pas);
    }
    menuLCD.displayMessage(buffer, NULL);
    Serial.print("Pas moteur : ");
    Serial.println(pas);
    pasMoteurStart = pasMoteur;
}

void loop()
{       
    char buffer[32];
    
    if (moteurStatut != MoteurStatutArret) {
        // Stop la course du moteur avec les butées
        // seulement si le moteur est en train de tourner...
        if (digitalRead(FDC_HOME_PIN) == LOW) {
            menuLCD.clear();
            menuLCD.displayMessage("Home", NULL, 1000);
            Moteur1.stop();
            moteurStatut = MoteurStatutArret;
        }
        if (digitalRead(FDC_END_PIN) == LOW) {
            menuLCD.clear();
            menuLCD.displayMessage("Fin de course", NULL, 1000);
            Moteur1.stop();
            moteurStatut = MoteurStatutArret;
        }
    }
    
    switch(menuLCD.getKey()) {
    
    // Up: Avant
    case 0:
        if (digitalRead(FDC_HOME_PIN) == LOW) {
            menuLCD.displayMessage("Home", NULL, 1000);
        } else {
            deplaceMoteur(pasMoteur, true);
        }
        break; 

    // Down: Arriere
    case 3: // Direction 2 en manuel
        if (digitalRead(FDC_END_PIN) == LOW) {
            menuLCD.displayMessage("Fin de course", NULL, 1000);
        } else {
            deplaceMoteur(pasMoteur, false);
        }
        break;
            
    // Right: Pas +
    case 1:
        pasMoteur += parMoteurDelta;
        sprintf(buffer, "Pas + : %d", pasMoteur);
        menuLCD.displayMessage(buffer, NULL, 1000);
        break;
              
    // Left: Pas -
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
        menuLCD.clear();
        menuLCD.displayMessage("Arret moteur", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
        break;


    default:
        break;
    }
    if (!Moteur1.stopped()) {
        if (moteurStart == 0) {
            moteurStart = millis();
        }
        SC.delay(200);
        if (Moteur1.stopped()) {
            menuLCD.clear();
            Serial.print("Moteur : ");
            Serial.println(millis() - moteurStart);
            Serial.print("Temps par pas : ");
            Serial.println((millis() - moteurStart) / (double)pasMoteurStart);
            moteurStart = 0;
        }
    }
    menuLCD.loop();
}

