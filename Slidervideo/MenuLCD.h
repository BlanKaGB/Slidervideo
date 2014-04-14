#include <Arduino.h>
#include <Deuligne.h> // LCD library include

#define LINE_SIZE 16
struct __MenuLCDMenuItem;
typedef struct __MenuLCDMenuItem MenuLCDMenuItem;

class MenuLCD
{
public:
    MenuLCD(void);
    void init(void);
    int8_t getKey(unsigned char *repeatCoef);
    void displayMessage(const char *line1, const char *line2 = NULL, unsigned int delay = 0, boolean shouldClearScreen = true);
    void loop(void);
    void clear(void);
    MenuLCDMenuItem *lastSelectedMenuItem(void);
    unsigned int selectedMenuIdentifer(void);
    MenuLCDMenuItem *menuItemUnderCursor(void);
    MenuLCDMenuItem *getMainMenuItem(void);
    void setMainMenuItem(MenuLCDMenuItem *newMainMenu);
    MenuLCDMenuItem *addMenuItem(MenuLCDMenuItem *menuItem, const char *title, unsigned int identifier);
    MenuLCDMenuItem *addSubMenuItem(MenuLCDMenuItem *menuItem, const char *title, unsigned int identier);
    void changeMenuItemTitle(MenuLCDMenuItem *menuItem, const char *title);
    void editValue(long value, long delta, long minValue, long maxValue, const char *message, void (*callback)(long value));
    void debugPrint(void);
    
private:
    Deuligne             _deuligne;
    unsigned long        _messageTime;
    unsigned long        _messageDelay;
    int8_t               _lastKey;
    unsigned long        _lastKeyTime;
    char                 _firstLine[LINE_SIZE + 1];
    char                 _secondLine[LINE_SIZE + 1];
    MenuLCDMenuItem      *_mainMenuItem;
    MenuLCDMenuItem      *_firstLineMenuItem;
    int                  _selectedLine;
    MenuLCDMenuItem      *_lastSelectedMenuItem;
    
    long                  _editedValue;
    long                  _editedValueDelta;
    long                  _editedValueMax;
    long                  _editedValueMin;
    void                  (*_editedValueCallback)(long value);
    
    MenuLCDMenuItem *createMenuItem(const char *title, unsigned int identifier);
    void freeMenuItem(MenuLCDMenuItem *menuItem);
    void selectMenuItem(MenuLCDMenuItem *menuItem);
    void displayEditedValue(void);
    void updateMenu(void);
};
