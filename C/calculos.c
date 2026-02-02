#include <stdio.h>
#include "structs.h"

int obtener_limite_capa(int Z) {
    if (Z <= 2) return 2;
    if (Z <= 10) return 10;
    if (Z <= 18) return 18;
    if (Z <= 36) return 36;
    if (Z <= 54) return 54;
    if (Z <= 86) return 86;
    return 118;
}

double obtener_mulliken_config(AtomoData *atomo, int num_electrones) {
    int carga = atomo->Z - num_electrones;

    if (carga >= 0) {
        if (carga < 120 && atomo->tiene_datos_carga[carga]) {
            return atomo->estados[carga].mulliken_ev;
        }
        printf("Error: No hay datos en JSON para %s con carga +%d\n", atomo->simbolo, carga);
        return -1.0;
    }

    else {
        int max_e = obtener_limite_capa(atomo->Z);
        int limite_negativo = atomo->Z - max_e;

        if (carga < limite_negativo) {
            printf("Error: Carga %d imposible para %s (Limite: %d)\n", carga, atomo->simbolo, limite_negativo);
            return -1.0;
        }

        if (carga == limite_negativo) {
            return 0.0; 
        }

        return atomo->afinidad_neutra / 2.0;
    }
}