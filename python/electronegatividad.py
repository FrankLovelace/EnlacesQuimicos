import csv
import json
import math
import os

RYDBERG_EV = 13.605693
ALPHA_AFFINITY = 0.30  


def valence_n(Z):
    if Z <= 2: return 1
    elif Z <= 10: return 2
    elif Z <= 18: return 3
    elif Z <= 36: return 4
    elif Z <= 54: return 5
    elif Z <= 86: return 6
    else: return 7

def get_max_capacity(Z):

    limits = [2, 10, 18, 36, 54, 86, 118]
    for limit in limits:
        if Z <= limit:
            return limit
    return 118 

def zeff(Z, charge=0):
    shielding = 0.35 * (Z - 1)
    return max(1.0, Z - shielding - charge)

def ionization_energy(Z, charge):
    n = valence_n(Z)
    Zeff = zeff(Z, charge)
    return RYDBERG_EV * (Zeff ** 2) / (n ** 2)

def electron_affinity(I):
    return ALPHA_AFFINITY * I

def mulliken(I, A):
    return (I + A) / 2

# --- MAIN ---
def main():
    elements = {}

    base_dir = os.path.dirname(os.path.abspath(__file__))
    csv_path = os.path.join(base_dir, "elementos.csv")
    json_path = os.path.join(base_dir, "electronegatividad_mulliken.json")

    print(f"Procesando: {csv_path}...")

    try:
        with open(csv_path, newline="", encoding="utf-8") as f:
            nombres_columnas = ["simbolo", "nombre", "numero_atomico"]
            reader = csv.DictReader(f, fieldnames=nombres_columnas)
            
            for row in reader:
                try:
                    Z = int(row["numero_atomico"])
                    symbol = row["simbolo"]
                    name = row["nombre"]

                    elements[symbol] = {
                        "nombre": name,
                        "Z": Z,
                        "cargas": {}
                    }

                    max_electrones = get_max_capacity(Z) 
                    
                    for e in range(1, max_electrones + 1):
                        
                        charge = Z - e
                        
                        I = ionization_energy(Z, charge)
                        A = electron_affinity(I)
                        chi = mulliken(I, A)

                        elements[symbol]["cargas"][str(charge)] = {
                            "electrones": e,
                            "ionization_eV": round(I, 4),
                            "affinity_eV": round(A, 4),
                            "mulliken": round(chi, 4)
                        }
                        
                except ValueError:
                    continue

        with open(json_path, "w", encoding="utf-8") as f:
            json.dump(elements, f, indent=4, ensure_ascii=False)

        print(f"Base de datos generada exitosamente en: {json_path}")

    except FileNotFoundError:
        print("ERROR: No se encontro 'elementos.csv'.")

if __name__ == "__main__":
    main()