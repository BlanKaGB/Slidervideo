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

SnootorStep Moteur1;
int pasMoteur = 400;
int parMoteurDelta = 1000;
int fdc = 13;               // delaration de l'entree pin 13 "fdc_home"
int fdc_home = 12;          // delaration de l'entree pin 12 "fdc"
MenuLCD menuLCD;
unsigned int moteurStart = 0;
unsigned int pasMoteurStart = 0;

MoteurStatut moteurStatut = MoteurStatutArret;

void setup()
{
    Serial.begin(115200);
    Wire.begin(); // join i2c
  
    Moteur1.init(2000,200,1,MOTOR_MODE_FULLSTEP); // moteur 200 pas/tour
    
    pinMode(13,INPUT_PULLUP); // declare la pin digital 13 en entree "fdc_home"
    pinMode(12,INPUT_PULLUP); // declare la pin digital 13 en entree "fdc"
    
    menuLCD.init();
    menuLCD.displayMessage("Pret...", NULL, 2000);
}

void deplaceMoteur(boolean avance)
{
    char buffer[32];

    if (avance) {
        Moteur1.forward(pasMoteur);
        moteurStatut = MoteurStatutAvant;
        sprintf(buffer, "Arriere : %d       ", pasMoteur);
    } else {
        Moteur1.back(pasMoteur);
        moteurStatut = MoteurStatutArriere;
        sprintf(buffer, "Avance : %d       ", pasMoteur);
    }
    menuLCD.displayMessage(buffer, NULL);
    Serial.print("Pas moteur : ");
    Serial.println(pasMoteur);
    pasMoteurStart = pasMoteur;
}

void loop()
{       
    int fdc_home = digitalRead(13);
    int fdc = digitalRead(12);
    char buffer[32];
    
    Serial.println(fdc_home);
    if (fdc_home == LOW) {
        menuLCD.clear();
        menuLCD.displayMessage("Home", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    }
    Serial.println(fdc);
    if (fdc == LOW) {
        menuLCD.clear();
        menuLCD.displayMessage("Fin de course", NULL, 1000);
        Moteur1.stop();
        moteurStatut = MoteurStatutArret;
    }
    
    switch(menuLCD.getKey()) {
    
    // Up: Avant
    case 0:
        deplaceMoteur(true);
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

    // Down: Arriere
    case 3: // Direction 2 en manuel
        deplaceMoteur(false);
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

