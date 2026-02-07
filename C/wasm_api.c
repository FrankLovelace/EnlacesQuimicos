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
        return "{\"error\": \"Elemento no encontrado en DB\"}";
    }

    char nomA[50], nomB[50];
    memset(nomA, 0, 50); memset(nomB, 0, 50);
    obtener_nombre_completo(atA->simbolo, nomA);
    obtener_nombre_completo(atB->simbolo, nomB);

    double mA = obtener_mulliken_config(atA, eA);
    double mB = obtener_mulliken_config(atB, eB);

    if (mA < 0 || mB < 0) {
        free(atA); free(atB);
        return "{\"error\": \"Sin información en NIST para esta carga.\"}";
    }

    double delta_m = fabs(mA - mB);
    double prom_m = (mA + mB) / 2.0;
    double ic = calcular_porcentaje_ic(delta_m);
    
    int LA, LB;
    int k = calcular_multiplicidad(eA, eB, atA->Z, atB->Z, &LA, &LB);

    const char* tipo = determinar_tipo_enlace_mulliken(delta_m, prom_m, mA, mB, atA->Z, atB->Z, k);
    const char* k_texto = nombre_multiplicidad(k);

    static char resultado[2048];
    snprintf(resultado, sizeof(resultado), 
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

EMSCRIPTEN_KEEPALIVE
const char* obtener_info_elemento_wasm(const char* simb, int electrones) {
    if (!simb || strlen(simb) == 0) return "{\"error\": \"404\"}";

    AtomoData *at = cargar_elemento_json(simb);
    if (!at) return "{\"error\": \"404\"}";

    char nombre[50];
    memset(nombre, 0, sizeof(nombre));
    obtener_nombre_completo(at->simbolo, nombre);

    double m_actual = obtener_mulliken_config(at, electrones);
    int carga = at->Z - electrones;

    static char json_buffer[512];
    snprintf(json_buffer, sizeof(json_buffer), 
            "{\"nombre\": \"%s\", \"z\": %d, \"mulliken\": %.4f, \"pauling_ref\": %.2f, \"carga\": %d}", 
            nombre, at->Z, m_actual, at->pauling_referencia, carga);

    free(at);
    return json_buffer;
}