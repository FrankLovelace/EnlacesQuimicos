import csv
import json
import os
import glob

DIRECTORIO_BASE = os.path.dirname(os.path.abspath(__file__))
CARPETA_ELEMENTOS = os.path.join(DIRECTORIO_BASE, "elementos_individuales")
ARCHIVO_AFINIDAD = os.path.join(DIRECTORIO_BASE, "AfinidadElectronica.csv")
OUTPUT_JSON = os.path.join(DIRECTORIO_BASE, "base_datos_quimica_final.json")

def limpiar_dato_nist(texto):
    if not texto: return None
    t = texto.replace('="', '').replace('"', '').replace('=', '')
    t = t.replace('(', '').replace(')', '').replace('[', '').replace(']', '')
    t = t.strip()
    return t if t != "" else None

def cargar_afinidades():
    diccionario = {}
    if not os.path.exists(ARCHIVO_AFINIDAD):
        print(f"!!! ERROR: No se encontro {ARCHIVO_AFINIDAD}")
        return diccionario

    with open(ARCHIVO_AFINIDAD, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) >= 2:
                simb = row[0].strip()
                try:
                    val = float(row[1])
                    diccionario[simb] = val
                except ValueError: continue
    return diccionario

def procesar_archivo_elemento(ruta_filepath, mapa_afinidades, db_global):
    filename = os.path.basename(ruta_filepath)
    energias_por_carga = {} 
    
    z_del_elemento = None
    simbolo_del_elemento = None

    with open(ruta_filepath, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 10: continue
            if "At. num" in row[0] or "Notes:" in row[0]: continue
            
            raw_z = limpiar_dato_nist(row[0])
            raw_carga = limpiar_dato_nist(row[2])
            raw_simbolo = limpiar_dato_nist(row[4])
            raw_energia = limpiar_dato_nist(row[9])

            if not raw_z or not raw_carga or not raw_energia: continue

            try:
                carga = int(raw_carga)
                energia = float(raw_energia)
                
                if simbolo_del_elemento is None:
                    z_del_elemento = int(raw_z)
                    simbolo_del_elemento = raw_simbolo
                
                energias_por_carga[carga] = energia
            except ValueError: continue

    if simbolo_del_elemento is None:
        print(f"  [!] Saltado: {filename} (No se detecto simbolo)")
        return

    afinidad_neutra_csv = mapa_afinidades.get(simbolo_del_elemento, 0.0)
    estados_calculados = {}
    
    cargas_ordenadas = sorted(energias_por_carga.keys())
    
    for carga in cargas_ordenadas:
        ie_actual = energias_por_carga[carga]
        
        if carga == 0:
            ea_usada = afinidad_neutra_csv
            metodo = "CSV"
        else:
            carga_previa = carga - 1
            if carga_previa in energias_por_carga:
                ea_usada = energias_por_carga[carga_previa]
                metodo = f"IE(+{carga_previa})"
            else:
                continue

        mulliken = (ie_actual + ea_usada) / 2
        estados_calculados[str(carga)] = {
            "carga": carga,
            "ie_ev": ie_actual,
            "ea_ev": ea_usada,
            "mulliken_ev": round(mulliken, 4),
            "info_ea": metodo
        }

    db_global[simbolo_del_elemento] = {
        "Z": z_del_elemento,
        "simbolo": simbolo_del_elemento,
        "estados": estados_calculados
    }
    print(f"  [OK] {simbolo_del_elemento} (Z={z_del_elemento}) - {len(estados_calculados)} estados.")

def main():
    print("--- PROCESAMIENTO ---")
    
    afinidades = cargar_afinidades()
    patron = os.path.join(CARPETA_ELEMENTOS, "*.csv") 
    archivos = glob.glob(patron)
    
    if not archivos:
        print(f"ERROR: No se encontraron archivos en: {CARPETA_ELEMENTOS}")
        return

    print(f"Archivos encontrados: {len(archivos)}")
    db_desordenada = {}

    for archivo in archivos:
        procesar_archivo_elemento(archivo, afinidades, db_desordenada)

    print("Ordenando por Z...")
    db_ordenada = dict(sorted(db_desordenada.items(), key=lambda x: x[1]['Z']))

    with open(OUTPUT_JSON, 'w', encoding='utf-8') as f:
        json.dump(db_ordenada, f, indent=4, ensure_ascii=False)

    print("-" * 30)
    print(f"PROCESO COMPLETADO. Revisa {OUTPUT_JSON}")

if __name__ == "__main__":
    main()