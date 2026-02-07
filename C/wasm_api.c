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

    if (!atA || !atB) {
        if (atA) free(atA);
        if (atB) free(atB);
        return "{\"error\": \"Uno de los elementos no existe en la base de datos.\"}";
    }

    char nomA[50], nomB[50];
    obtener_nombre_completo(atA->simbolo, nomA);
    obtener_nombre_completo(atB->simbolo, nomB);

    double mA = obtener_mulliken_config(atA, eA);
    double mB = obtener_mulliken_config(atB, eB);

    if (mA == -2.0 || mB == -2.0) {
        free(atA); free(atB);
        return "{\"error\": \"No se permiten aniones (electrones > Z).\"}";
    }
    if (mA == -1.0 || mB == -1.0) {
        free(atA); free(atB);
        return "{\"error\": \"Sin información en NIST para una de las cargas seleccionadas.\"}";
    }

    double delta_m = fabs(mA - mB);
    double prom_m = (mA + mB) / 2.0;
    
    double ic = calcular_porcentaje_ic(delta_m);
    
    int LA, LB;
    int k = calcular_multiplicidad(eA, eB, atA->Z, atB->Z, &LA, &LB);

    const char* tipo = determinar_tipo_enlace_mulliken(delta_m, prom_m, mA, mB, atA->Z, atB->Z, k);
    const char* k_texto = nombre_multiplicidad(k);

    static char resultado[2048];
    sprintf(resultado, 
        "{"
        "\"nomA\": \"%s\", \"zA\": %d, \"mA\": %.4f, \"brazosA\": %d, "
        "\"nomB\": \"%s\", \"zB\": %d, \"mB\": %.4f, \"brazosB\": %d, "
        "\"delta\": %.4f, \"promedio\": %.4f, \"ic\": %.2f, "
        "\"k\": %d, \"k_nom\": \"%s\", \"tipo\": \"%s\""
        "}",
        nomA, atA->Z, mA, LA,
        nomB, atB->Z, mB, LB,
        delta_m, prom_m, ic, 
        k, k_texto, tipo);

    free(atA); free(atB);
    return resultado;
}