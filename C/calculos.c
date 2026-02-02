#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
double convertir_a_pauling(double mulliken_ev) {
    return (0.374 * mulliken_ev) + 0.17;
}

const char* clasificar_ion(int carga) {
    if (carga > 0) return "Cation";
    if (carga < 0) return "Anion";
    return "Neutro";
}

double calcular_delta_chi(double chiA, double chiB) {
    return fabs(chiA - chiB);
}

double calcular_promedio_chi(double chiA, double chiB) {
    return (chiA + chiB) / 2.0;
}

double calcular_porcentaje_ic(double delta_chi) {
    return (1.0 - exp(-0.25 * pow(delta_chi, 2))) * 100.0;
}

const char* determinar_tipo_enlace(double delta_chi, double promedio_chi) {
    if (promedio_chi < 2.0) {
        return "Metálico";
    } else if (delta_chi > 1.7) {
        return "Iónico";
    } else if (delta_chi > 0.4 && delta_chi <= 1.7) {
        return "Covalente Polar";
    } else {
        return "Covalente No Polar";
    }
}