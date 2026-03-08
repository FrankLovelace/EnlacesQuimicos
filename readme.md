
# NIST Chemistry Bonding Simulator (WASM Edition)

Este proyecto es un simulador de enlaces químicos que utiliza datos experimentales del **NIST (Atomic Spectra Database)** para predecir el tipo de enlace, la multiplicidad y el carácter iónico entre dos átomos en cualquier estado de oxidación (cationes y neutros).
Es además mi primer proyecto dentro del mundo del software científico

El motor principal está escrito en **C** y compilado a **WebAssembly (WASM)** para lograr una precisión física de alto rendimiento en entornos web.

## Características Principales

### 1. Motor de Decisión de Enlace (Lógica VAK)
El simulador implementa el **Triángulo de Van Arkel-Ketelaar (VAK)** mapeado con coordenadas de energía en electronvoltios (eV):
- **Eje X (Promedio de Mulliken):** Determina la deslocalización (Metálico vs No-Metálico).
- **Eje Y (Diferencia de Mulliken):** Determina la polaridad y el carácter iónico.
- **Zonas definidas:** Iónico, Covalente Polar (Sólido/Red), Covalente Polar (Molecular), Covalente No Polar y Metálico/Aleación.

### 2. Rigor Físico (Mulliken vs Pauling)
A diferencia de los simuladores básicos que usan tablas estáticas de Pauling, este software:
- Utiliza la **Escala de Mulliken** basada en Energías de Ionización ($IE$) y Afinidades Electrónicas ($EA$) reales del NIST.
- **Electronegatividad Dinámica:** El valor de Mulliken cambia según el número de electrones (carga) del átomo.
- **Referencia de Pauling:** Proporciona la escala de Pauling como referencia comparativa estática para el estado neutro.

### 3. Tratamiento de Casos Especiales
El algoritmo detecta y maneja múltiples incidencias químicas:
- **Metales de Transición:** Ignora la regla del octeto simple y fuerza enlaces de tipo red (multiplicidad simple) para óxidos y aleaciones.
- **Gases Nobles:** Detecta fuerzas de Van der Waals e implementa **Hipervalencia** cuando se enfrentan a elementos altamente oxidantes (F, O, Cl).
- **El "Piso de la Realidad":** Filtra estados inestables donde la repulsión electrostática impide el enlace ($<1.0$ eV).
- **Prohibición de Aniones:** El sistema restringe la simulación a estados neutros y cationes debido a la inestabilidad de los aniones aislados en el modelo de Mulliken.

## Arquitectura Técnica

### Flujo de Datos
1.  **Python Scraper/Parser:** Procesa archivos CSV sucios del NIST y los unifica en un JSON estructurado por símbolo químico y carga.
2.  **C Logic Engine:** 
    - `db_manager.c`: Carga los JSON y maneja una tabla maestra incrustada para nombres y referencias.
    - `calculos.c`: Contiene las fórmulas de Hannay-Smyth, transiciones suaves de carácter iónico y lógica de multiplicidad $k$.
3.  **WASM Bridge:** `wasm_api.c` expone funciones de C a JavaScript mediante Emscripten.

### Estructura del Proyecto
- `/C`: Código fuente del motor de cálculo.
    - `data/`: Contiene la base de datos JSON empaquetada.
- `/python`: Scripts de procesamiento y limpieza de datos NIST.
- `/web`: Interfaz de usuario (HTML5, Canvas para el triángulo VAK).

## Compilación y Despliegue

### Requisitos
- [Emscripten SDK (emsdk)](https://emscripten.org/)
- GCC (para la versión de consola)

### Compilar para Web (WASM)
Ejecutar en PowerShell/Terminal dentro de la carpeta `C/`:
```bash
emcc wasm_api.c db_manager.c calculos.c cJSON.c -o quimica.js \
-s 'EXPORTED_RUNTIME_METHODS=["cwrap","FS"]' \
-s 'EXPORTED_FUNCTIONS=["_calcular_enlace_wasm","_obtener_info_elemento_wasm"]' \
-s MODULARIZE=1 -s EXPORT_NAME="createModule" \
-s ALLOW_MEMORY_GROWTH=1 --preload-file data -lm
```

### Compilar versión Consola (`main.c`)
```bash
gcc -finput-charset=UTF-8 -fexec-charset=UTF-8 main.c db_manager.c calculos.c cJSON.c -o buscador.exe -lm
```

## Notas de Versión 
- Añadida tabla maestra de 118 elementos incrustada en el binario.
- Implementada transición suave (fade-in/out) entre Pauling y Hannay-Smyth para %IC.
- Soporte para Bloques D (Metales) y F (Lantánidos/Actínidos).
