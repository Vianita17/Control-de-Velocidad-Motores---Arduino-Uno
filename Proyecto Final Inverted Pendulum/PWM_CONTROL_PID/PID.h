#ifndef PID_H
#define PID_H

#include <Arduino.h>

// Anti-windup: si la salida satura, el último paso integral se revierte para
// evitar que el integrador acumule error sin efecto real en la planta.
//
// Uso típico:
//   PID ctrl(KP, KI, KD);          // Límites por defecto: ±255
//   float salida = ctrl.calcular(setpoint, medicion, dt);

class PID {
public:
    // ── Constructor
    // kp, ki, kd  : ganancias proporcional, integral y derivativa
    // salidaMin   : límite inferior de la salida (default -255 → reversa máxima)
    // salidaMax   : límite superior de la salida (default +255 → avance máximo)
    PID(float kp, float ki, float kd,
        float salidaMin = -255.0f,
        float salidaMax =  255.0f);

    // ── calcular
    // Ejecuta un ciclo del PID y retorna la señal de control calculada.
    //   setpoint : valor deseado de la variable controlada
    //   actual   : valor medido de la variable controlada
    //   dt       : paso de tiempo en segundos desde el ciclo anterior
    // Retorna: señal de control acotada en [salidaMin, salidaMax]
    float calcular(float setpoint, float actual, float dt);

    // ── reset
    // Reinicia el estado interno del controlador (acumulador integral y
    // error previo). Llamar cuando el sistema cambia de modo o se detiene.
    void reset();

private:
    float _kp, _ki, _kd;		// Ganancias del controlador PID.
    float _salidaMin;		// Límite inferior de la salida
    float _salidaMax;		// Límite superior de la salida
    float _errorPrev;		// Error del ciclo anterior (para término D)
    float _integral;		// Acumulador del término integral
};

#endif
