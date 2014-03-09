#include <Deuligne.h> // LCD library include

class MenuLCD
{
public:
    void init();
    int8_t getKey();
    void displayMessage(const char *message, unsigned int delay = 0);
    void loop(void);
    void clear(void);
    
private:
    Deuligne             _deuligne;
    unsigned long        _messageTime;
    unsigned long        _messageDelay;
    unsigned char        _lastKey;
};
