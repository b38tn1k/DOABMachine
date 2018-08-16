#ifndef Button_h
#define Button_h

class Button{
  public:
    void init(int mode, uint8_t _pin);
    void checkState();
    bool triggered;
    int tag;
    uint8_t state;
    bool isDown();
  private:
    uint8_t pin;
    uint8_t defaultState;  
};

#endif
