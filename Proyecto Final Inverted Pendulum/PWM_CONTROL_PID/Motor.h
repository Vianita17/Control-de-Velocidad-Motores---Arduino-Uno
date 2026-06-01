#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h> 

// Pines de control del puente H
#define PIN_DIR_HIGH 4 
#define PIN_DIR_LOW   7

class Motor {
    private:
        uint8_t _pinPWM;
        uint8_t _pinEncA;
        uint8_t _pinEncB;
        uint8_t _pinDirHigh;
        uint8_t _pinDirLow;
        
        volatile bool _ASet, _BSet, _APrev, _BPrev;
        volatile long _LeftEncoderTicks; 

    public:
        // Constructor para recibir los pines de dirección
        Motor(uint8_t pinPWM, uint8_t pinEncA, uint8_t pinEncB, uint8_t pinDirHigh, uint8_t pinDirLow);
        void begin();
        void setVelocidad(int velocidad);
        void HandleLeftMotorInterruptA();
        void HandleLeftMotorInterruptB();
        int parseEncoder(); 

        // Métodos de acceso para el lazo de control
        long getPulsos();   
        void resetPulsos(); 
};

#endif