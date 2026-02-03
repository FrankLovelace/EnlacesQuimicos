#include <emscripten.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "structs.h"

EMSCRIPTEN_KEEPALIVE
const char* calcular_enlace_wasm(const char* simbA, int eA, const char* simbB, int eB) {
    AtomoData *atA = cargar_elemento_json(simbA);
    AtomoData *atB = cargar_elemento_json(simbB);

    if (!atA || !atB) return "{\"error\": \"Elemento no encontrado\"}";

    double mA = obtener_mulliken_config(atA, eA);
    double mB = obtener_mulliken_config(atB, eB);
    
    double delta_m = fabs(mA - mB);
    double prom_m = (mA + mB) / 2.0;
    double ic = calcular_porcentaje_ic(0.374 * delta_m);
    
    int LA, LB;
    int k = calcular_multiplicidad(eA, eB, atA->Z, atB->Z, &LA, &LB);

    static char resultado[1024];
    sprintf(resultado, 
        "{\"mA\": %.4f, \"mB\": %.4f, \"delta\": %.4f, \"promedio\": %.4f, \"ic\": %.2f, \"k\": %d, \"tipo\": \"%s\", \"k_nom\": \"%s\", \"brazosA\": %d, \"brazosB\": %d}",
        mA, mB, delta_m, prom_m, ic, k, determinar_tipo_enlace_mulliken(delta_m, prom_m), nombre_multiplicidad(k), LA, LB);

    free(atA); free(atB);
    return resultado;
}