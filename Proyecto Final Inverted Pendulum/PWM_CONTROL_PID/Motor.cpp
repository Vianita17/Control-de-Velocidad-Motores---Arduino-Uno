#include "Motor.h"
#include <digitalWriteFast.h> // Requerido para alto rendimiento.

Motor::Motor(uint8_t pinPWM, uint8_t pinEncA, uint8_t pinEncB, uint8_t pinDirHigh, uint8_t pinDirLow) {
    _pinPWM     = pinPWM;
    _pinEncA    = pinEncA;
    _pinEncB    = pinEncB;
    _pinDirHigh = pinDirHigh;
    _pinDirLow  = pinDirLow;

    _LeftEncoderTicks = 0;
    _ASet = _BSet = _APrev = _BPrev = false;
}

void Motor::begin() {
    pinMode(_pinDirHigh, OUTPUT);
    pinMode(_pinDirLow,  OUTPUT);
    pinMode(_pinPWM, OUTPUT);
    analogWrite(_pinPWM, 0);

    pinMode(_pinEncA, INPUT_PULLUP);
    pinMode(_pinEncB, INPUT_PULLUP);
    
    _APrev = digitalReadFast(_pinEncA);
    _BPrev = digitalReadFast(_pinEncB);
}

void Motor::setVelocidad(int velocidad) {
    if (velocidad > 255)  velocidad = 255;
    if (velocidad < -255) velocidad = -255;

    if (velocidad >= 0) {
        digitalWrite(_pinDirHigh, HIGH);
        digitalWrite(_pinDirLow,  LOW);
        analogWrite(_pinPWM, velocidad);
    } else {
        digitalWrite(_pinDirHigh, LOW);
        digitalWrite(_pinDirLow,  HIGH);
        analogWrite(_pinPWM, abs(velocidad)); 
    }
}

// ─── Encoder de cuadratura (QEI X4)
int Motor::parseEncoder() {
    // ─── Lógica de ParseEncoder integrada (X4)
    if(_APrev && _BPrev){
        if(!_ASet && _BSet) return 1;
        else if(_ASet && !_BSet) return -1;
    } else if(!_APrev && _BPrev){
        if(!_ASet && !_BSet) return 1;
        else if(_ASet && _BSet) return -1;
    } else if(!_APrev && !_BPrev){
        if(_ASet && !_BSet) return 1;
        else if(!_ASet && _BSet) return -1;
    } else if(_APrev && !_BPrev){
        if(_ASet && _BSet) return 1;
        else if(!_ASet && !_BSet) return -1;
    }
    
    return 0;
}

void Motor::HandleLeftMotorInterruptA(){
    _BSet = digitalReadFast(_pinEncB);
    _ASet = digitalReadFast(_pinEncA);
  
    _LeftEncoderTicks += parseEncoder();
  
    _APrev = _ASet;
    _BPrev = _BSet;
}

void Motor::HandleLeftMotorInterruptB(){
    _BSet = digitalReadFast(_pinEncB);
    _ASet = digitalReadFast(_pinEncA);
  
    _LeftEncoderTicks += parseEncoder();
  
    _APrev = _ASet;
    _BPrev = _BSet;
}

long Motor::getPulsos() {
    return _LeftEncoderTicks;
}

void Motor::resetPulsos() {
    _LeftEncoderTicks = 0;
}