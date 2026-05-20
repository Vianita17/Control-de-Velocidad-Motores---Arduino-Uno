import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def sintonizar_pid(path_csv, pwm_input=255):
    df = pd.read_csv(path_csv)
    
    # ultimos datos
    df_step = df[df['time'] >= 14800].copy()
    df_step['time_s'] = (df_step['time'] - df_step['time'].min()) / 1000.0
    
    # suavizar
    df_step['rpm_smooth'] = df_step['rpm'].rolling(window=5, center=True).mean().bfill().ffill()

    # derivada
    df_step['derivada'] = np.gradient(df_step['rpm_smooth'], df_step['time_s'])
    
    R = df_step['derivada'].max() 
    idx_R = df_step['derivada'].idxmax()
    t_R = df_step.loc[idx_R, 'time_s']
    rpm_R = df_step.loc[idx_R, 'rpm_smooth']
    K = df_step['rpm_smooth'].max()
    
    # L = t - (y/m)
    L = t_R - (rpm_R / R)
    if L < 0: L = 0.001 # Evitar valores negativos por ruido
    
    # R_norm que es la pendiente dividida por la entrada (255)
    R_norm = R / pwm_input
    
    kp = 1.2 / (R_norm * L)
    ti = 2 * L
    td = 0.5 * L
    
    ki = kp / ti
    kd = kp * td

    print(f"Parámetros Identificados")
    print(f"Pendiente Máxima (R): {R:.2f} RPM/s")
    print(f"Retardo (L): {L:.4f} s")
    print(f"Ganancia Máxima (K): {K:.2f} RPM")
    print(f"Valores Sugeridos para PID")
    print(f"Kp: {kp:.4f}")
    print(f"Ki: {ki:.4f}")
    print(f"Kd: {kd:.4f}")

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10), sharex=True)
    
    # GRÁFICA 1: RPM y Recta Tangente
    ax1.plot(df_step['time_s'], df_step['rpm'], 'b.', alpha=0.3, label='Datos Originales')
    ax1.plot(df_step['time_s'], df_step['rpm_smooth'], 'r-', label='Datos Suavizados')
    
    # SOLUCIÓN AL ERROR: Graficar la recta usando plot tradicional con los datos calculados
    t_plot = df_step['time_s']
    tangente_linea = R * (t_plot - t_R) + rpm_R
    ax1.plot(t_plot, tangente_linea, 'g--', label=f'Tangente (R={R:.2f})')
    
    # CORRECCIÓN DE SINTAXIS: Se usa .set_* para los objetos Axes (ax1)
    ax1.set_title('Identificación por Curva de Reacción (Punto de Inflexión)')
    ax1.set_ylabel('RPM')
    ax1.set_ylim(0, K * 1.2) # Ajustar escala para ver mejor
    ax1.legend()
    ax1.grid(True)

    # GRÁFICA 2: La Derivada (Aceleración)
    ax2.plot(df_step['time_s'], df_step['derivada'], 'm-', label='Derivada (dRPM/dt)')
    # Marcamos explícitamente el punto máximo de la derivada
    ax2.scatter([t_R], [R], color='green', s=100, zorder=5, label=f'Punto de Inflexión Máximo')
    
    ax2.set_title('Análisis de la Derivada (Aceleración del Motor)')
    ax2.set_xlabel('Tiempo (s)')
    ax2.set_ylabel('Aceleración [RPM/s]')
    ax2.legend()
    ax2.grid(True)

    plt.tight_layout()
    plt.show()

    # SAVE_txt(kp, ki, kd, R, L, K)

def SAVE_txt(kp, ki, kd, R, L, K, filename="valores_PID.txt"):
    try:
        with open(filename, 'w') as f:
            f.write("REPORTE DE IDENTIFICACION DEL MOTOR\n")
            f.write("Parametros del Sistema\n")
            f.write(f"Pendiente Maxima (R): {R:.4f} RPM/s\n")
            f.write(f"Tiempo de Retardo (L): {L:.4f} s\n")
            f.write(f"Ganancia de Estado Estable (K): {K:.4f} RPM\n\n")
            f.write("Constantes PID Sugeridas (Ziegler-Nichols)\n")
            f.write(f"Kp: {kp:.4f}\n")
            f.write(f"Ki: {ki:.4f}\n")
            f.write(f"Kd: {kd:.4f}\n")
        print(f"\n[OK] Reporte guardado exitosamente en: {filename}")
    except Exception as e:
        print(f"\n[ERROR] No se pudo guardar el archivo: {e}")

archivo = r"C:\...\MOTOR2.csv"
sintonizar_pid(archivo, pwm_input=255)
