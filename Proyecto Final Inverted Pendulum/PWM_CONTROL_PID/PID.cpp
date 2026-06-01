#include "PID.h"
// ── Constructor 
// Inicializa las ganancias, los límites de salida y el estado interno a cero.
PID::PID(float kp, float ki, float kd, float salidaMin, float salidaMax)
    : _kp(kp),
      _ki(ki),
      _kd(kd),
      _salidaMin(salidaMin),
      _salidaMax(salidaMax),
      _errorPrev(0.0f),
      _integral(0.0f)
{}

// ── calcular 
// Algoritmo PID en tiempo discreto con anti-windup.
// error = setpoint - actual
float PID::calcular(float setpoint, float actual, float dt) {

    // ── Cálculo del error 
    float error = setpoint - actual;

    // ── Término proporcional 
    float P = _kp * error;

    // ── Término integral
    _integral += error * dt;
    float I = _ki * _integral;

    // ── Término derivativO
    float D = _kd * (error - _errorPrev) / dt;

    // Guardar error actual para el próximo ciclo
    _errorPrev = error;

    // ── Salida total 
    float salida = P + I + D;

    // ── Anti-windup
    if (salida > _salidaMax) {
        salida = _salidaMax;
        _integral -= error * dt;    // Revertir el último incremento integral
    }
    else if (salida < _salidaMin) {
        salida = _salidaMin;
        _integral -= error * dt;    // Revertir el último incremento integral
    }

    return salida;
}

// ── reset
// Borra el estado interno del controlador.
void PID::reset() {
    _errorPrev = 0.0f;
    _integral  = 0.0f;
}
