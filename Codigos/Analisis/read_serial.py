import serial
import pandas as pd
from datetime import datetime
import time

#cambiar port
PORT = "COM32"
BAUD = 9600

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

data = []

print("Capturando datos... (presiona Ctrl+C para detener)")

try:
    while True:
        line = ser.readline().decode(errors='ignore').strip()

        print(f"Recibido: {line}")

        if not line or "time" in line:
            continue

        values = line.split(",")

        if len(values) == 2:
            data.append(values)

except KeyboardInterrupt:
    print("\nDetenido. Guardando archivo...")

finally:
    ser.close()

    if data:
        df = pd.DataFrame(data, columns=[
            "time","rpm"
        ])

        filename = f"dataset_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
        filename = f"MOTOR2.csv"
        df.to_csv(filename, index=False)

        print(f"Archivo guardado: {filename}")
    else:
        print("No se capturaron datos")