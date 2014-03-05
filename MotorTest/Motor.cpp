#include "Motor.h"
#include <Wire.h>
#include <Arduino.h>
#include "TimerOne.h"

#define I2C_ADDRESS        0x20
#define MOTOR_PWM1_PIN     11
#define MOTOR_PWM2_PIN     3
#define MOTOR_ADDR1        0x10
#define MOTOR_ADDR2        0x11

Motor::Motor()
{
    _currentStep = 0;
//    Timer1.initialize();
}

void Motor::run(unsigned long int steps, float time)
{
}

void Motor::stop(void)
{
    this->i2cCommand(MOTOR_ADDR1, 0x00);
    this->i2cCommand(MOTOR_ADDR2, 0x00);
}

void Motor::nextStep(void)
{
    _currentStep = (_currentStep + 1) % 8;
    this->halfStep();
}

void Motor::previousStep(void)
{
    _currentStep = (_currentStep + 7) % 8;
    this->halfStep();
}

void Motor::halfStep(void)
{
    switch(_currentStep) {
    case 0:
        this->i2cCommand(MOTOR_ADDR1, 0x00, 0xF0);
        break;
    case 1:
        this->i2cCommand(MOTOR_ADDR1, 0x0F, 0xF0);
        break;
    case 2:
        this->i2cCommand(MOTOR_ADDR1, 0x0F, 0x00);
        break;
    case 3:
        this->i2cCommand(MOTOR_ADDR1, 0x0F, 0x0F);
        break;
    case 4:
        this->i2cCommand(MOTOR_ADDR1, 0x00, 0x0F);
        break;
    case 5:
        this->i2cCommand(MOTOR_ADDR1, 0xF0, 0x0F);
        break;
    case 6:
        this->i2cCommand(MOTOR_ADDR1, 0xF0, 0x00);
        break;
    case 7:
        this->i2cCommand(MOTOR_ADDR1, 0xF0, 0xF0);
        break;
    }
}

void Motor::init(void)
{
    pinMode(MOTOR_PWM1_PIN, OUTPUT);
    pinMode(MOTOR_PWM2_PIN, OUTPUT);
    analogWrite(MOTOR_PWM1_PIN, 255);
    analogWrite(MOTOR_PWM2_PIN, 255);
    
    Wire.begin();
    this->i2cCommand( 0xe, 0xff);                    // 0f Internal oscilator disabled. All output are static WITHOUT PWM
    this->i2cCommand( 0xf, 0x10);			// blink 0 on
    this->i2cCommand( 0x6, 0x00);			// input and output config.
    this->i2cCommand( 0x7, 0x00);			// ...
    this->i2cCommand( 0x2, 0xff);			// global intensity reg.
    this->i2cCommand(0x3, 0xff);
    this->i2cCommand(0xe, 0xff);			// config bit
    this->i2cCommand(0x10,0x00);                        // zero out pwms on register 0x10
    this->i2cCommand(0x11,0x00);                        // zero out pwms on register 0x11
    this->i2cCommand(0x12,0x00);                        // zero out pwms on register 0x12
    this->i2cCommand(0x13,0x00);                        // zero out pwms on register 0x13
    this->i2cCommand(0x14,0x00);                        // zero out pwms on register 0x14
    this->i2cCommand(0x15,0x00);                        // zero out pwms on register 0x15
    this->i2cCommand(0x16,0x00);                        // zero out pwms on register 0x16
    this->i2cCommand(0x17,0x00);                        // zero out pwms on register 0x17
}

void Motor::i2cCommand(short address, short data)
{
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission();
}

void Motor::i2cCommand(short address1, short data1, short data2)
{
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(address1);
    Wire.write(data1);
    Wire.write(data2);
    Wire.endTransmission();
}

