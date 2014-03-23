#include <Deuligne.h> // LCD library include

#define LINE_SIZE 16
struct __MenuLCDMenuItem;
typedef struct __MenuLCDMenuItem MenuLCDMenuItem;

class MenuLCD
{
public:
    MenuLCD(void);
    void init(void);
    int8_t getKey(void);
    void displayMessage(const char *line1, const char *line2 = NULL, unsigned int delay = 0);
    void loop(void);
    void clear(void);
    unsigned int selectedMenuIdentifer(void);
    MenuLCDMenuItem *menuItemUnderCursor(void);
    MenuLCDMenuItem *getMainMenuItem(void);
    MenuLCDMenuItem *addMenuItem(MenuLCDMenuItem *menuItem, const char *title, unsigned int identifier);
    MenuLCDMenuItem *addSubMenuItem(MenuLCDMenuItem *menuItem, const char *title, unsigned int identier);
    void changeMenuItemTitle(MenuLCDMenuItem *menuItem, const char *title);
    void debugPrint(void);
    
private:
    Deuligne             _deuligne;
    unsigned long        _messageTime;
    unsigned long        _messageDelay;
    unsigned char        _lastKey;
    char                 _firstLine[LINE_SIZE + 1];
    char                 _secondLine[LINE_SIZE + 1];
    MenuLCDMenuItem      *_mainMenuItem;
    MenuLCDMenuItem      *_firstLineMenuItem;
    int                  _selectedLine;
    
    MenuLCDMenuItem *createMenuItem(const char *title, unsigned int identifier);
    void freeMenuItem(MenuLCDMenuItem *menuItem);
    void selectMenuItem(MenuLCDMenuItem *menuItem);
    void updateMenu(void);
};
