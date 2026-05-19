#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h> 

#define PIN_DIR_HIGH 4 
#define PIN_DIR_LOW   7

class Motor {
    private:
        uint8_t _pinPWM;
        uint8_t _pinEncA;
        uint8_t _pinEncB;
        
        volatile bool _ASet, _BSet, _APrev, _BPrev;
        volatile long _LeftEncoderTicks; // Sigue siendo privada, lo cual es buena práctica

    public:
        Motor(uint8_t pinPWM, uint8_t pinEncA, uint8_t pinEncB);
        void begin();
        void setVelocidad(uint8_t velocidad);
        void HandleLeftMotorInterruptA();
        void HandleLeftMotorInterruptB();
        int parseEncoder(); 

        // ─── NUEVOS MÉTODOS PARA CORREGIR EL ENCAPSULAMIENTO ───
        long getPulsos();   // Para leer los ticks de forma externa
        void resetPulsos(); // Para reiniciar el contador
};

#endif