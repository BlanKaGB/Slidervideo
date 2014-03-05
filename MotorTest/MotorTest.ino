#include <Wire.h>
#include "Motor.h"
#include "TimerOne.h"
#include <Deuligne.h> // LCD library include

#define DELAY 500

Motor moteur;
Deuligne lcd; // lcd object declaration

void setup()
{
    Serial.begin(115200);
    Serial.println("init");
    moteur.init();
    lcd.init(); // LCD init
    lcd.clear(); // Clear Display
    lcd.backLight(true); // Backlight ON
}

unsigned long lastStep = 0;
int currentDirection = 0;

void doStep()
{
    lastStep = millis();
    switch(currentDirection) {
    case 1:
        moteur.nextStep();
        lcd.setCursor(0, 1);
        lcd.print("Avant    ");
        break;
    case -1:
        moteur.previousStep();
        lcd.setCursor(0, 1);
        lcd.print("Arriere  ");
        break;
    default:
        lcd.setCursor(0, 1);
        lcd.print("         ");
        break;
    }
}

void loop()
{
    static int8_t oldKey = -2;
    int key;
    
    key = lcd.get_key();
    if (oldKey != key || millis() - lastStep > DELAY) {
        oldKey = key;
        switch(key) {
        case 0:
            currentDirection = 1;
            doStep();
            currentDirection = 0;
            break;
        case 3:
            currentDirection = -1;
            doStep();
            currentDirection = 0;
            break;
        case 1:
            currentDirection = 1;
            doStep();
            break;
        case 2:
            currentDirection = -1;
            doStep();
            break;
        case 4:
            currentDirection = 0;
            doStep();
            break;
        }
    }
    if (millis() - lastStep > DELAY) {
        doStep();
    }
    lcd.setCursor(0,0);
    lcd.print("Step : ");
    lcd.print(moteur.currentStep());
}
