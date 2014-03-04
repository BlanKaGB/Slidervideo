#include <Wire.h>
#include "Motor.h"
#include "TimerOne.h"
#include <Deuligne.h> // LCD library include

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

void loop()
{
    moteur.nextStep();
    lcd.clear();
    lcd.print("Step : ");
    lcd.print(moteur.currentStep());
    delay(500);
}
