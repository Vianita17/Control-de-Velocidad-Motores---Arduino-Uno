/*
 * Motor.cpp — Implementación de la clase Motor.
 *
 * El control de velocidad usa analogWrite() de Arduino, que internamente
 * configura el Timer0 en modo Fast PWM y escribe en el registro OCR0A/OCR0B.
 * Esto es equivalente al acceso directo por ensamblador usado anteriormente,
 * con la ventaja de que es portable y no requiere conocer la dirección exacta
 * de los registros del AVR.
 */

#include "Motor.h"
#include <digitalWriteFast.h> // Requerido para alto rendimiento
// ─────────────────────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Almacena los pines asignados e inicializa el contador de encoder en cero.
 * La configuración del hardware se realiza en begin().
 */
Motor::Motor(uint8_t pinPWM, uint8_t pinEncA, uint8_t pinEncB) {
    _pinPWM  = pinPWM;
    _pinEncA = pinEncA;
    _pinEncB = pinEncB;

    _LeftEncoderTicks = 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Inicialización del hardware
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Configura todos los pines del motor y fija la dirección de giro.
 *
 * Los pines de dirección (PIN_DIR_HIGH y PIN_DIR_LOW) son constantes compartidas
 * definidas en Motor.h. Si se llama begin() en dos instancias, estos pines se
 * configurarán dos veces con los mismos valores, lo cual es inofensivo.
 *
 * El pin de PWM se declara OUTPUT pero su señal es controlada por analogWrite(),
 * no por digitalWrite(). Arduino configura automáticamente el Timer0 la primera
 * vez que se llama analogWrite() en los pines 5 o 6.
 *
 * Los pines del encoder usan INPUT_PULLUP para evitar lecturas flotantes cuando
 * el encoder no está activo o durante el arranque del sistema.
 */
void Motor::begin() {
    // ── Pines de dirección (fijos, compartidos entre motores) ─────────────────
    pinMode(PIN_DIR_HIGH, OUTPUT);
    pinMode(PIN_DIR_LOW,  OUTPUT);

    digitalWrite(PIN_DIR_HIGH, HIGH); // Fija el sentido de giro
    digitalWrite(PIN_DIR_LOW,  LOW);  // Complemento: siempre opuesto a PIN_DIR_HIGH

    // ── Pin de PWM ────────────────────────────────────────────────────────────
    pinMode(_pinPWM, OUTPUT);
    analogWrite(_pinPWM, 0); // Motor detenido al iniciar (ciclo de trabajo = 0)

    // ── Pines del encoder ─────────────────────────────────────────────────────
    // INPUT_PULLUP: asegura nivel lógico definido cuando el encoder no está activo
    pinMode(_pinEncA, INPUT_PULLUP);
    pinMode(_pinEncB, INPUT_PULLUP);
}

// ─────────────────────────────────────────────────────────────────────────────
// Control de velocidad
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Escribe el ciclo de trabajo en el pin PWM usando analogWrite().
 *
 * analogWrite() en los pines 5 y 6 escribe directamente en los registros
 * OCR0B y OCR0A del Timer0, respectivamente. La frecuencia resultante es
 * ~976 Hz (prescalador 64, resolución 8 bits).
 *
 * @param velocidad  0 = motor detenido, 255 = velocidad máxima.
 */
void Motor::setVelocidad(uint8_t velocidad) {
    analogWrite(_pinPWM, velocidad);
}

// ─────────────────────────────────────────────────────────────────────────────
// Encoder de cuadratura (QEI)
// ─────────────────────────────────────────────────────────────────────────────


int Motor::parseEncoder() {

    // Lógica de ParseEncoder integrada (X4)
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
    

}

void Motor:: HandleLeftMotorInterruptA(){
    _BSet = digitalReadFast(_pinEncB);
    _ASet = digitalReadFast(_pinEncA);
  
    _LeftEncoderTicks+=parseEncoder();
  
    _APrev = _ASet;
    _BPrev = _BSet;;
}

void Motor:: HandleLeftMotorInterruptB(){
  // Test transition;
    _BSet = digitalReadFast(_pinEncB);
    _ASet= digitalReadFast(_pinEncA);
  
    _LeftEncoderTicks+=parseEncoder();
  
    _APrev = _ASet;
    _BPrev = _BSet;
}

long Motor::getPulsos() {
    return _LeftEncoderTicks;
}

void Motor::resetPulsos() {
    _LeftEncoderTicks = 0;
}



