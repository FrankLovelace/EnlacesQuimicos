#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "structs.h"

int es_metal_transicion(int Z) {
    return ((Z >= 21 && Z <= 30) || (Z >= 39 && Z <= 48) || (Z >= 71 && Z <= 80));
}

int obtener_limite_capa(int Z) {
    if (Z <= 2) return 2;
    if (Z <= 10) return 10;
    if (Z <= 18) return 18;
    if (Z <= 36) return 36;
    if (Z <= 54) return 54;
    if (Z <= 86) return 86;
    return 118;
}

int es_lantanido_actinido(int Z) {
    return ((Z >= 57 && Z <= 70) || (Z >= 89 && Z <= 102));
}

int es_gas_noble_z(int Z) {
    return (Z == 2 || Z == 10 || Z == 18 || Z == 36 || Z == 54 || Z == 86);
}

int obtener_limite_capa_estricto(int Z) {
    return Z; 
}

int obtener_capacidad_capa(int num_electrones) {
    if (num_electrones <= 2) return 2;
    return 8;
}

int calcular_valencia_v(int num_electrones, int Z) {
    if (es_metal_transicion(Z)) return 2;
if (es_lantanido_actinido(Z)) return 3;
if (es_gas_noble_z(Z)) return 0;

    int cores[] = {86, 54, 36, 18, 10, 2, 0};
    for (int i = 0; i < 7; i++) {
        if (num_electrones > cores[i]) {
            int valencia_bruta = num_electrones - cores[i];

            if ((Z > 30 && Z <= 36) || (Z > 48 && Z <= 54) || (Z > 80 && Z <= 86)) {
                return valencia_bruta - 10;
            }
            return valencia_bruta;
        }
        else if (num_electrones == cores[i] && num_electrones != 0) {
             return (cores[i] == 2) ? 2 : 8; 
        }
    }
    return num_electrones;
}

double obtener_mulliken_config(AtomoData *atomo, int num_electrones) {
    int carga = atomo->Z - num_electrones;

    if (carga < 0) {
        return -2.0; 
    }

    if (carga < 120 && atomo->tiene_datos_carga[carga]) {
        return atomo->estados[carga].mulliken_ev;
    }

    return -1.0; 
}

double calcular_delta_chi(double chiA, double chiB) { return fabs(chiA - chiB); }
double calcular_promedio_chi(double chiA, double chiB) { return (chiA + chiB) / 2.0; }
double calcular_porcentaje_ic(double delta_chi) { return (1.0 - exp(-0.25 * pow(delta_chi, 2))) * 100.0; }

const char* determinar_tipo_enlace_mulliken(double delta_m, double prom_m, int ZA, int ZB) {
     if (es_gas_noble_z(ZA) || es_gas_noble_z(ZB)) {
        if (delta_m > 0.1) return "Covalente Polar (Gas Noble)";
        else return "Fuerzas de Van der Waals";
    }

    if (prom_m < 4.89) return "Metálico";
    if (delta_m > 4.10) return "Iónico";
    if (delta_m > 0.61) return "Covalente Polar";
    return "Covalente No Polar";
}

int calcular_multiplicidad(int eA, int eB, int ZA, int ZB, int *LA, int *LB) {
    int vA = calcular_valencia_v(eA, ZA);
    int vB = calcular_valencia_v(eB, ZB);
    
    if (es_metal_transicion(ZA) || es_lantanido_actinido(ZA)) {
        *LA = vA; 
    } else {
        int cap = (eA <= 2) ? 2 : 8;
        int huecos = cap - vA;
        *LA = (huecos < vA) ? huecos : vA;
    }

    if (es_metal_transicion(ZB) || es_lantanido_actinido(ZB)) {
        *LB = vB;
    } else {
        int cap = (eB <= 2) ? 2 : 8;
        int huecos = cap - vB;
        *LB = (huecos < vB) ? huecos : vB;
    }

    if (*LA < 0) *LA = 0;
    if (*LB < 0) *LB = 0;

    int k = (*LA < *LB) ? *LA : *LB;
    return (k > 3) ? 3 : k;
}

const char* nombre_multiplicidad(int k) {
    if (k == 1) return "Simple (sigma)";
    if (k == 2) return "Doble (sigma + pi)";
    if (k >= 3) return "Triple (sigma + 2pi)";
    return "No definido";
}

const char* clasificar_ion(int carga) {
    if (carga > 0) return "Catión";
    if (carga < 0) return "Anión";
    return "Neutro";
}