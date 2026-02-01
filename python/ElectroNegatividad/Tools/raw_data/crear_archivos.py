import csv
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

ARCHIVO_ENTRADA = os.path.join(BASE_DIR, 'elementos.csv')

CARPETA_SALIDA = os.path.join(BASE_DIR, 'elementos_individuales')

if not os.path.exists(CARPETA_SALIDA):
    os.makedirs(CARPETA_SALIDA)
    print(f"Carpeta '{CARPETA_SALIDA}' creada.")

try:
    with open(ARCHIVO_ENTRADA, mode='r', encoding='utf-8', newline='') as f:
        reader = csv.reader(f)

        count = 0
        for row in reader:
            if len(row) < 3:
                continue

            simbolo = row[0].strip()
            nombre = row[1].strip()
            numero = row[2].strip()

            nombre_archivo = f"{numero}_{nombre}.csv"
            ruta_completa = os.path.join(CARPETA_SALIDA, nombre_archivo)

            with open(ruta_completa, 'w', encoding='utf-8', newline='') as out_file:
                out_file.write("Z,Elemento,Simbolo\n")
                out_file.write(f"{numero},{nombre},{simbolo}\n")

            print(f"Generado: {nombre_archivo}")
            count += 1

    print(f"\n¡Listo! Se crearon {count} archivos en la carpeta 'elementos_individuales'.")

except FileNotFoundError:
    print("Error: No encontré 'elementos.csv' en la carpeta del script.")
except Exception as e:
    print(f"Ocurrió un error: {e}")
