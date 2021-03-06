#include <Arduino.h>
#include <EEPROM.h>
#include "Sauvegarde.h"

#define TOKEN           "bonjour"
#define VERSION         (int)3

void Sauvegarde::defaultValues(void)
{
    _pasMoteur = 1000;
    _pasMoteurDelta = 1000;
    _vitesse = 1000;
    _vitesseDelta = 1000;
}

boolean Sauvegarde::load(void)
{
    char buffer[sizeof(TOKEN)];
    int version;
    int address = 0;
    boolean result = false;
    
    this->defaultValues();
    address += this->readBuffer(buffer, sizeof(TOKEN), address);
    address += this->readValue(&version, address);
    if (memcmp(buffer, TOKEN, sizeof(TOKEN)) == 0) {
        Serial.println("EEPROM token valide");
    }
    if (version == VERSION) {
        Serial.println("EEPROM valide valide");
    }
    if (memcmp(buffer, TOKEN, sizeof(TOKEN)) == 0 && version == VERSION) {
        address += this->readValue(&_pasMoteur, address);
        address += this->readValue(&_pasMoteurDelta, address);
        address += this->readValue(&_vitesse, address);
        address += this->readValue(&_vitesseDelta, address);
        result = true;
    }
    return result;
}

void Sauvegarde::save(void)
{
    int address = 0;

    address += this->writeBuffer(TOKEN, sizeof(TOKEN), address);
    address += this->writeValue(VERSION, address);
    address += this->writeValue(_pasMoteur, address);
    address += this->writeValue(_pasMoteurDelta, address);
    address += this->writeValue(_vitesse, address);
    address += this->writeValue(_vitesseDelta, address);
    Serial.println("EEPROM sauvegarde");
}

unsigned int Sauvegarde::readBuffer(void *buffer, unsigned int size, unsigned int address)
{
    unsigned int originalSize = size;
    
    while (size > 0) {
        ((byte *)buffer)[0] = EEPROM.read(address);
        buffer = (byte *)buffer + 1;
        address++;
        size--;
    }
    return originalSize;
}

unsigned int Sauvegarde::readValue(int *value, unsigned int address)
{
    return this->readBuffer(value, sizeof(*value), address);
}

unsigned int Sauvegarde::readValue(long *value, unsigned int address)
{
    return this->readBuffer(value, sizeof(*value), address);
}

long Sauvegarde::pasMoteur(void)
{
    return _pasMoteur;
}

void Sauvegarde::setPasMoteur(long value)
{
    _pasMoteur = value;
}

long Sauvegarde::pasMoteurDelta(void)
{
    return _pasMoteurDelta;
}

void Sauvegarde::setPasMoteurDelta(long value)
{
    _pasMoteurDelta = value;
}

long Sauvegarde::vitesse(void)
{
    return _vitesse;
}

void Sauvegarde::setvitesse(long value)
{
    _vitesse = value;
}

long Sauvegarde::vitesseDelta(void)
{
    return _vitesseDelta;
}

void Sauvegarde::setvitesseDelta(long value)
{
    _vitesseDelta = value;
}

unsigned int Sauvegarde::writeBuffer(const void *buffer, unsigned int size, unsigned int address)
{
    unsigned int originalSize = size;
    
    while(size > 0) {
        EEPROM.write(address, ((byte *)buffer)[0]);
        buffer = (byte *)buffer + 1;
        address++;
        size--;
    }
    return originalSize;
}

unsigned int Sauvegarde::writeValue(int value, unsigned int address)
{
    return writeBuffer(&value, sizeof(value), address);
}

unsigned int Sauvegarde::writeValue(long value, unsigned int address)
{
    return writeBuffer(&value, sizeof(value), address);
}
