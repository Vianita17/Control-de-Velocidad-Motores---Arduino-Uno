#include "Motor.h"
#include "PID.h"
#include <EnableInterrupt.h>

// Hardware de Motores
Motor motor1(5, 2, A0, 4, 7); 
Motor motor2(6, 3, A1, 8, 9);

void m1InterruptA() { motor1.HandleLeftMotorInterruptA(); }
void m1InterruptB() { motor1.HandleLeftMotorInterruptB(); }
void m2InterruptA() { motor2.HandleLeftMotorInterruptA(); }
void m2InterruptB() { motor2.HandleLeftMotorInterruptB(); }

// Sensor de Angulo
#define PIN_POT A2
#define POT_A   -0.1579f
#define POT_B   0.0062f
long x_0 = 0;
float Y_0 = 0.0f;

// Variables de Tiempo
const int INTERVALO_MS = 10;
unsigned long tiempoPrevioPID = 0;

// Constantes PID Lazo Interno
const float KP_VEL = 4.9676;
const float KI_VEL = 21.0818;
const float KD_VEL = 0.2926;

// Constantes PID Lazo Externo
const float KP_ANG = 150.0;
const float KI_ANG = 0.0;  
const float KD_ANG = 5.0;  

// Instancias PID
PID pidAngulo(KP_ANG, KI_ANG, KD_ANG, -120.0f, 120.0f);
PID pidVelocidadM1(KP_VEL, KI_VEL, KD_VEL, -255.0f, 255.0f);
PID pidVelocidadM2(KP_VEL, KI_VEL, KD_VEL, -255.0f, 255.0f);

float calcularRPM(Motor &m, float dt);

void setup() {
    Serial.begin(115200);
    
    motor1.begin();
    motor2.begin();

    enableInterrupt(digitalPinToInterrupt(2), m1InterruptA, CHANGE);
    enableInterrupt(digitalPinToInterrupt(A0), m1InterruptB, CHANGE);
    enableInterrupt(digitalPinToInterrupt(3), m2InterruptA, CHANGE); 
    enableInterrupt(digitalPinToInterrupt(A1), m2InterruptB, CHANGE);

    // Calibracion inicial
    x_0 = analogRead(PIN_POT);
    Y_0 = POT_A + POT_B * (float)(x_0);
    
    tiempoPrevioPID = millis();
    Serial.println("--- Sistema Iniciado en Cascada ---");
}

void loop() {
    unsigned long tiempoActual = millis();

    // Lazo de control
    if (tiempoActual - tiempoPrevioPID >= INTERVALO_MS) {
        float dt = (tiempoActual - tiempoPrevioPID) / 1000.0f;
        tiempoPrevioPID = tiempoActual;

        // Lazo Externo: Entrada de posicion y calculo de setpoint
        long lecturaADC = analogRead(PIN_POT);
        float anguloRad = (POT_A + POT_B * (float)(lecturaADC)) - Y_0;
        
        float setpointRPM = pidAngulo.calcular(0.0f, anguloRad, dt);

        // Lazo Interno: Lectura de velocidad y calculo de PWM
        float rpm1 = calcularRPM(motor1, dt);
        float rpm2 = calcularRPM(motor2, dt);

        float pwm1 = pidVelocidadM1.calcular(setpointRPM, rpm1, dt);
        float pwm2 = pidVelocidadM2.calcular(-setpointRPM, -rpm2, dt);

        // Salidas de potencia
        motor1.setVelocidad((int)pwm1);
        motor2.setVelocidad((int)pwm2);
        
        // Telemetria
        Serial.print("Angulo: "); Serial.print(anguloRad, 4);
        Serial.print(" | Target_RPM: "); Serial.print(setpointRPM);
        Serial.print(" | Act_RPM1: "); Serial.print(rpm1); 
        Serial.print(" | Act_RPM2: "); Serial.println(rpm2); 
    }
}

// Funcion auxiliar de RPM
float calcularRPM(Motor &m, float dt) {
    long pulsosExtraidos = m.getPulsos(); 
    m.resetPulsos();

    float revoluciones = (float)pulsosExtraidos / 4000.0f;
    float rpm = (revoluciones * 60.0f) / dt;
    
    return rpm;
}