#include <Deuligne.h> // LCD library include

#define LINE_SIZE 16

class MenuLCD
{
public:
    void init();
    int8_t getKey();
    void displayMessage(const char *line1, const char *line2 = NULL, unsigned int delay = 0);
    void loop(void);
    void clear(void);
    
private:
    Deuligne             _deuligne;
    unsigned long        _messageTime;
    unsigned long        _messageDelay;
    unsigned char        _lastKey;
    char                 _firstLine[LINE_SIZE + 1];
    char                 _secondLine[LINE_SIZE + 1];
};
