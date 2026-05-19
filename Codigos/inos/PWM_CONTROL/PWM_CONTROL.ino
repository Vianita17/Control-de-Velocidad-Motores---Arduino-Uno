#include "Motor.h"

// ─── Configuración de Hardware ───────────────────────────────────────────────
Motor motor1(6, 2, A0); 
Motor motor2(5, 3, A1); 

// ─── Funciones Puente (Wrappers) para las Interrupciones ─────────────────────
// Estas funciones globales sí pueden ser procesadas por attachInterrupt
void m1InterruptA() { motor1.HandleLeftMotorInterruptA(); }
void m1InterruptB() { motor1.HandleLeftMotorInterruptB(); }
void m2InterruptA() { motor2.HandleLeftMotorInterruptA(); }
void m2InterruptB() { motor2.HandleLeftMotorInterruptB(); }

// ─── Constantes del PID (Calculadas en Python) ───────────────────────────────
const float KP = 4.9676;
const float KI = 21.0818;
const float KD = 0.2926;

// ─── Variables de Control ────────────────────────────────────────────────────
const int INTERVALO_MS = 10;
unsigned long tiempoPrevio = 0;

float setpointRPM = 100.0; 
float errorPrev1 = 0, integral1 = 0;
float errorPrev2 = 0, integral2 = 0;

void setup() {
    Serial.begin(115200);
    
    motor1.begin();
    motor2.begin();


    attachInterrupt(digitalPinToInterrupt(2), m1InterruptA, CHANGE);
    attachInterrupt(digitalPinToInterrupt(A0), m1InterruptB, CHANGE);

    // attachInterrupt(digitalPinToInterrupt(3), m2InterruptA, CHANGE); 
    // attachInterrupt(digitalPinToInterrupt(A1), m2InterruptB, CHANGE);

    tiempoPrevio = millis(); 
}

void loop() {
    unsigned long tiempoActual = millis();
    unsigned long lastTicks = 0;

    // Lazo de control de tiempo constante (10ms)
    if (tiempoActual - tiempoPrevio >= INTERVALO_MS) {
        float dt = (tiempoActual - tiempoPrevio) / 1000.0; 

        int current_ticks = motor1.getPulsos(); 
        lastTicks = current_ticks;

        float rpm1 = calcularRPM(motor1, dt);

        int pwm1 = calcularPID(setpointRPM, rpm1, errorPrev1, integral1, dt);

        motor1.setVelocidad(pwm1);
        
        Serial.print("RPM-MOTOR1: ");    
        Serial.println(rpm1); 

        tiempoPrevio = tiempoActual;
    }

    // Leer potenciómetro para ajustar velocidades
    int pot = analogRead(A2);
    setpointRPM = map(pot, 0, 1023, 0, 130); 
}

float calcularRPM(Motor &m, float dt) {
    // CORREGIDO: Ahora estos métodos ya existen en la interfaz pública de Motor.h
    long pulsosExtraidos = m.getPulsos(); 
    m.resetPulsos();

    float revoluciones = (float)pulsosExtraidos / 4000.0;
    float rpm = revoluciones * (60.0 / dt);
    
    return rpm;
}

/**
 * Algoritmo PID con Anti-windup
 */
int calcularPID(float setpoint, float actual, float &errorPrev, float &integral, float dt) {
    float error = setpoint - actual;
    float P = KP * error;
    integral += error * dt;
    float I = KI * integral;
    float D = KD * (error - errorPrev) / dt;
    errorPrev = error;
    
    float salida = P + I + D;
    
    if (salida > 255) {
        salida = 255;
        integral -= error * dt;
    } else if (salida < 0) {
        salida = 0;
        integral -= error * dt;
    }
    
    return (int)salida;
}