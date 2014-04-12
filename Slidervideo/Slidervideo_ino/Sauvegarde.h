#include <Arduino.h>

class Sauvegarde
{
public:
    long pasMoteur(void);
    void setPasMoteur(long value);
    long pasMoteurDelta(void);
    void setPasMoteurDelta(long value);
    long vitesseDelta(void);
    void setvitesseDelta(long value);
    
    boolean load(void);
    void save(void);
    
private:
    long          _pasMoteur;
    long          _pasMoteurDelta;
    long          _vitesseDelta;
    
    void defaultValues(void);
    
    unsigned int readBuffer(void *buffer, unsigned int size, unsigned int address);
    unsigned int readValue(int *value, unsigned int address);
    unsigned int readValue(long *value, unsigned int address);
    
    unsigned int writeBuffer(const void *buffer, unsigned int size, unsigned int address);
    unsigned int writeValue(int value, unsigned int address);
    unsigned int writeValue(long value, unsigned int address);
};
