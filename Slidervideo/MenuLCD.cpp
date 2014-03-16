#include "MenuLCD.h"
#include <Arduino.h>

#define LCD_LENGTH 16

typedef struct __MenuLCDMenuItem {
    unsigned int identifier;
    char title[LCD_LENGTH];
    struct __MenuLCDMenuItem *parentMenuItem;
    struct __MenuLCDMenuItem *nextMenuItem;
    struct __MenuLCDMenuItem *subMenuItem;
} MenuLCDMenuItem;

MenuLCD::MenuLCD(void)
{
    _mainMenuItem = NULL;
}

void MenuLCD::init(void)
{
    _deuligne.init();
    _deuligne.clear();
    _deuligne.backLight(true);
    _lastKey = -1;
    _firstLine[0] = 0;
    _secondLine[0] = 0;
}

int8_t MenuLCD::getKey(void)
{
    int8_t key;

    key = _deuligne.get_key();
    if (key != _lastKey) {
        _lastKey = key;
    } else {
        key = -1;
    }
    return key;
}

inline void copyString(const char *origin, char *copy)
{
    if (!origin) {
        copy[0] = 0;
    } else {
        char ii = 0;
        
        while (origin[ii] != 0 && ii < LINE_SIZE) {
            copy[ii] = origin[ii];
            ii++;
        }
        copy[ii] = 0;
    }
}

void MenuLCD::displayMessage(const char *line1, const char *line2, unsigned int delay)
{
    _deuligne.clear();
    _deuligne.print(line1);
    if (line2) {
        _deuligne.setCursor(0, 1);
        _deuligne.print(line2);
    }
    _messageTime = millis();
    _messageDelay = delay;
    if (delay == 0) {
        copyString(line1, _firstLine);
        copyString(line2, _secondLine);
    } 
}

void MenuLCD::loop(void)
{
    if (_messageDelay != 0) {
        unsigned long currentTime = millis();
        
        if (currentTime < _messageTime) {
            // We got an overflow in millis() (every 50 days)
            // Let's recompute _messageTime with the new origin of millis()
            if (_messageTime + _messageDelay > _messageTime) {
                // in this case the delay was due just before the overflow
                // so we can't compute the new delay with the new origin of millis()
                // we have to trigger the timer now
                _messageDelay = 1;
            } else {
                // remove the delay that was in the previous origin
                _messageDelay -= ((unsigned long)-1) - _messageTime;
            }
            _messageTime = 0;
        }
        if (_messageTime + _messageDelay > _messageTime && _messageTime + _messageDelay < currentTime) {
            this->displayMessage(_firstLine, _secondLine, 0);
        }
    }
}

void MenuLCD::clear(void)
{
    this->displayMessage("", NULL, 0);
}

MenuLCDMenuItem *MenuLCD::getMainMenuItem(void)
{
    if (!_mainMenuItem) {
        _mainMenuItem = addMenuItem(NULL, NULL, 0);
    }
    return _mainMenuItem;
}

MenuLCDMenuItem *MenuLCD::createMenuItem(const char *title, unsigned int identier)
{
    MenuLCDMenuItem *result;
    
    result = (MenuLCDMenuItem *)malloc(sizeof(MenuLCDMenuItem));
    this->changeMenuItemTitle(result, title);
    result->identifier = identier;
    result->parentMenuItem = NULL;
    result->nextMenuItem = NULL;
    result->subMenuItem = NULL;
    return result;
}

MenuLCDMenuItem *MenuLCD::addMenuItem(MenuLCDMenuItem *menuItem, const char *title, unsigned int identifier)
{
    MenuLCDMenuItem *result = NULL;
    
    if (menuItem != _mainMenuItem) {
        result = this->createMenuItem(title, identifier);
        result->nextMenuItem = menuItem->nextMenuItem;
        menuItem->nextMenuItem = result;
        result->parentMenuItem = menuItem->parentMenuItem;
    }
    return result;
}

MenuLCDMenuItem *MenuLCD::addSubMenuItem(MenuLCDMenuItem *menuItem, const char *title, unsigned int identifier)
{
    MenuLCDMenuItem *result;
    
    if (menuItem->subMenuItem) {
        menuItem = menuItem->subMenuItem;
        while (menuItem->nextMenuItem != NULL) {
            menuItem = menuItem->nextMenuItem;
        }
        result = this->addMenuItem(menuItem, title, identifier);
    } else {
        result = this->createMenuItem(title, identifier);
        result->parentMenuItem = menuItem;
        menuItem->subMenuItem = result;
    }
    return result;
}

void MenuLCD::freeMenuItem(MenuLCDMenuItem *menuItem)
{
    while (menuItem) {
        MenuLCDMenuItem *nextMenuItem;
        
        freeMenuItem(menuItem->subMenuItem);
        nextMenuItem = menuItem->nextMenuItem;
        free(menuItem);
        menuItem = nextMenuItem;
    }
}

void MenuLCD::changeMenuItemTitle(MenuLCDMenuItem *menuItem, const char *title)
{
    int ii = 0;
    
    if (title) {
        int count = strlen(title);
        
        while (ii < count && ii < LCD_LENGTH) {
            menuItem->title[ii] = title[ii];
            ii++;
        }
    }
    menuItem->title[ii] = 0;
}

