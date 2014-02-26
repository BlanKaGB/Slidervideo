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
 * test
 */

#include <Wire.h> // I2C library include
#include <Deuligne.h> // LCD library include
#include <snootor.h>


typedef enum {
    MoteurStatutArret,
    MoteurStatutAvant,
    MoteurStatutArriere,
} MoteurStatut;

Deuligne lcd; // lcd object declaration
SnootorStep Moteur1;
int pasMoteur = 400;
int parMoteurDelta = 400;

MoteurStatut moteurStatut = MoteurStatutArret;

void setup()
{
    Wire.begin(); // join i2c
  
    Moteur1.init(750,200,2,MOTOR_MODE_HALFSTEP); // moteur 200 pas/tour au demi pas
    
    lcd.init(); // LCD init
    lcd.clear(); // Clear Display
    lcd.backLight(true); // Backlight ON
    lcd.setCursor(5,0); // Place cursor row 6, 1st line (counting from 0)
    lcd.print("Setup");
    lcd.setCursor(7,1); // Place cursor row 8, 2nd line (counting from 0)
    lcd.print("ok");
    delay(2000);
    lcd.clear();
    lcd.print("Pret ......");
}

void deplaceMoteur(boolean avance, const char *message)
{
    char buffer[32];

    lcd.setCursor(0,0);
    lcd.print(message);
    lcd.setCursor(0,1);
    sprintf(buffer, "Pas : %d       ", pasMoteur);
    lcd.print(buffer);
    delay(!Moteur1.stopped());
    
    if (avance) {
        Moteur1.forward(pasMoteur);
        moteurStatut = MoteurStatutAvant;
    } else {
        Moteur1.back(pasMoteur);
        moteurStatut = MoteurStatutArriere;
    }
    while(!Moteur1.stopped()) // arret du moteur quand les pas sont effectués
    SC.delay(200);
    SC.stop();
    lcd.clear();
}

void loop()
{
    static int8_t oldkey = -1;
    int8_t key;

    // Lis la valeur du Joystick et le converti en appui bouton
    key  = lcd.get_key();

    if (key != oldkey) {
        char buffer[32];
        
        // Si le bouton est appuyé
        oldkey = key;

        if (key >= 0) {
            // attention quand key est -1
            // il n’y a pas de message
            lcd.setCursor(0, 1);
        }

        lcd.setCursor(0, 1);
        switch(key) {
        
        // Pas +  
        case 0: 
            pasMoteur += parMoteurDelta;
            sprintf(buffer, "Pas + : %d", pasMoteur);
            lcd.print(buffer);
            delay(1000);
            break;
            

        // Avant
        case 1:
            deplaceMoteur(true, "AVANT  ");
            break;
                  
        // Arriere
        case 2:
            deplaceMoteur(false, "ARRIERE");
            break;

        // Pas -
        case 3: // Direction 2 en manuel
            pasMoteur -= parMoteurDelta;
            if (pasMoteur < parMoteurDelta) {
                pasMoteur = parMoteurDelta;
            }
            sprintf(buffer, "Pas - : %d", pasMoteur);
            lcd.print(buffer);
            delay(1000);
            break;
            

        // Arret
        case 4: // Stop
            lcd.print("Arret moteur");
            Moteur1.stop();
            moteurStatut = MoteurStatutArret;
            break;

        default:
            lcd.clear();
            break;
        }
    } else {
        
    }
}



