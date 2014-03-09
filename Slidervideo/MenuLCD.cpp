#include "MenuLCD.h"
#include <Arduino.h>

void MenuLCD::init()
{
    _deuligne.init();
    _deuligne.clear();
    _deuligne.backLight(true);
    _lastKey = -1;
}

int8_t MenuLCD::getKey()
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

void MenuLCD::displayMessage(const char *message, unsigned int delay)
{
    _deuligne.clear();
    _deuligne.print(message);
    _messageTime = millis();
    _messageDelay = delay;
}

void MenuLCD::loop(void)
{
    if (_messageDelay != 0) {
        unsigned long currentTime = millis();
        
        if (currentTime < _messageTime) {
            Serial.println("Bordel");
            
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
            _deuligne.clear();
            _messageTime = 0;
            _messageDelay = 0;
        }
    }
}

void MenuLCD::clear(void)
{
    _deuligne.clear();
}
