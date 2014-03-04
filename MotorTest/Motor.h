class Motor
{
public:
    Motor();
    void run(unsigned long int steps, float time);
    void stop(void);
    void nextStep(void);
    void init(void);
    unsigned char currentStep(void) { return _currentStep; };
    
protected:
    void i2cCommand(short address, short data);
    void i2cCommand(short address1, short data1, short data2);
    void halfStep(void);
    
    unsigned long int      _stepToRun;
    unsigned int           _stepTime;
    unsigned char          _currentStep;
};

