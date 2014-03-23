#include "MenuLCD.h"
#include <Arduino.h>

typedef struct __MenuLCDMenuItem {
    unsigned int identifier;
    char title[LINE_SIZE + 1];
    struct __MenuLCDMenuItem *parentMenuItem;
    struct __MenuLCDMenuItem *nextMenuItem;
    struct __MenuLCDMenuItem *subMenuItem;
} MenuLCDMenuItem;

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

inline MenuLCDMenuItem *previousMenuItem(MenuLCDMenuItem *menuItem)
{
    MenuLCDMenuItem *result = menuItem->parentMenuItem->subMenuItem;
    
    if (result == menuItem) {
        result = NULL;
    } else {
        while (result != NULL && result->nextMenuItem != menuItem) {
            result = result->nextMenuItem;
        }
    }
    return result;
}

MenuLCD::MenuLCD(void)
{
    _mainMenuItem = NULL;
    _firstLineMenuItem = NULL;
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

unsigned int MenuLCD::selectedMenuIdentifer(void)
{
    unsigned int result = 0;
    int8_t key = this->getKey();
    
    if (key != -1 && _firstLineMenuItem == NULL) {
        this->selectMenuItem(_mainMenuItem->subMenuItem);
    } else if (key == 1) { // up
        if (_selectedLine > 0) {
            _selectedLine--;
            this->updateMenu();
        } else {
            MenuLCDMenuItem *previous = previousMenuItem(_firstLineMenuItem);
            
            if (previous != NULL) {
                _firstLineMenuItem = previous;
                this->updateMenu();
            }
        }
    } else if (key == 2) { //down
        if (_selectedLine < 1) {
            _selectedLine++;
            this->updateMenu();
        } else {
            MenuLCDMenuItem *next = _firstLineMenuItem->nextMenuItem;
            
            if (next != NULL && next->nextMenuItem != NULL) {
                _firstLineMenuItem = next;
                this->updateMenu();
            }
        }
    } else if (key == 3) { // left
        if (_firstLineMenuItem->parentMenuItem == _mainMenuItem) {
            this->selectMenuItem(NULL);
        } else {
            _firstLineMenuItem = _firstLineMenuItem->parentMenuItem;
            _selectedLine = 0;
            this->updateMenu();
        }
    } else if (key == 0) { //right
        MenuLCDMenuItem *cursorMenuItem = this->menuItemUnderCursor();
        
        if (cursorMenuItem->subMenuItem) {
            this->selectMenuItem(cursorMenuItem->subMenuItem);
        }
    }
    return result;
}

void MenuLCD::selectMenuItem(MenuLCDMenuItem *menuItem)
{
    if (menuItem == NULL) {
        _firstLineMenuItem = NULL;
        _selectedLine = 0;
        _deuligne.noBlink();
        this->clear();
    } else {
        _firstLineMenuItem = menuItem;
        _selectedLine = 0;
        this->updateMenu();
    }
}

void MenuLCD::updateMenu(void)
{
    char line1[LINE_SIZE];
    char line2[LINE_SIZE];
    
    line1[0] = 0;
    line2[0] = 0;
    if (_firstLineMenuItem) {
        copyString(_firstLineMenuItem->title, line1);
        if (_firstLineMenuItem->subMenuItem != NULL) {
            line1[LINE_SIZE - 1] = '>';
        }
        if (_firstLineMenuItem->nextMenuItem) {
            copyString(_firstLineMenuItem->nextMenuItem->title, line2);
            if (_firstLineMenuItem->nextMenuItem->subMenuItem != NULL) {
                line2[LINE_SIZE - 1] = '>';
            }
        }
    }
    this->displayMessage(line1, line2, 0);
    _deuligne.blink();
    _deuligne.setCursor(0, _selectedLine);
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
        _mainMenuItem = this->createMenuItem(NULL, 0);
        _mainMenuItem->nextMenuItem = NULL;
        _mainMenuItem->parentMenuItem = NULL;
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
        
        while (ii < count && ii < LINE_SIZE) {
            menuItem->title[ii] = title[ii];
            ii++;
        }
    }
    while (ii < LINE_SIZE) {
        menuItem->title[ii] = ' ';
        ii++;
    }
    menuItem->title[ii] = 0;
}

MenuLCDMenuItem *MenuLCD::menuItemUnderCursor(void)
{
    MenuLCDMenuItem *result = _firstLineMenuItem;
    unsigned int ii = _selectedLine;
    
    if (result != NULL) {
        while (ii > 0) {
            result = result->nextMenuItem;
            ii--;
        }
    }
    return result;
}

void printMenu(MenuLCDMenuItem *menuItem, const char *prefix)
{
    char subPrefix[32];
    
    strcpy(subPrefix, prefix);
    subPrefix[strlen(prefix)] = '+';
    subPrefix[strlen(prefix)+1] = ' ';
    subPrefix[strlen(prefix)+2] = ' ';
    subPrefix[strlen(prefix)+3] = 0;
    while(menuItem) {
        Serial.print(prefix);
        Serial.print((unsigned int)menuItem, HEX);
        Serial.print(" ");
        Serial.print(menuItem->title);
        Serial.print(", ");
        Serial.println(menuItem->identifier);
        printMenu(menuItem->subMenuItem, subPrefix);
        menuItem = menuItem->nextMenuItem;
    }
}

void MenuLCD::debugPrint(void)
{
    printMenu(_mainMenuItem, "");
}
