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

int obtener_capacidad_capa(int num_electrones) {
    if (num_electrones <= 2) return 2;
    return 8;
}

int calcular_valencia_v(int num_electrones, int Z) {
    if (es_metal_transicion(Z)) return 2;
    int cores[] = {86, 54, 36, 18, 10, 2, 0};
    for (int i = 0; i < 7; i++) {
        if (num_electrones > cores[i]) return num_electrones - cores[i];
    }
    return num_electrones;
}

double obtener_mulliken_config(AtomoData *atomo, int num_electrones) {
    int carga = atomo->Z - num_electrones;
    if (carga >= 0) {
        if (carga < 120 && atomo->tiene_datos_carga[carga]) return atomo->estados[carga].mulliken_ev;
        return 0.0; 
    } else {
        int max_e = obtener_limite_capa(atomo->Z);
        if (carga < (atomo->Z - max_e)) return -1.0;
        if (carga == (atomo->Z - max_e)) return 0.0;
        return atomo->afinidad_neutra / 2.0;
    }
}

double calcular_delta_chi(double chiA, double chiB) { return fabs(chiA - chiB); }
double calcular_promedio_chi(double chiA, double chiB) { return (chiA + chiB) / 2.0; }
double calcular_porcentaje_ic(double delta_chi) { return (1.0 - exp(-0.25 * pow(delta_chi, 2))) * 100.0; }

const char* determinar_tipo_enlace_mulliken(double delta_m, double prom_m) {
    if (prom_m < 4.89 && delta_m < 4.10) return "Metálico";
    
    if (delta_m > 4.10) return "Iónico";
    
    if (delta_m > 0.61) return "Covalente Polar";
    return "Covalente No Polar";
}

int calcular_multiplicidad(int eA, int eB, int ZA, int ZB, int *LA, int *LB) {
    int vA = calcular_valencia_v(eA, ZA);
    int vB = calcular_valencia_v(eB, ZB);
    if (es_metal_transicion(ZA)) *LA = vA;
    else {
        int cap = obtener_capacidad_capa(eA);
        int h = cap - vA;
        *LA = (h < vA) ? h : vA;
    }
    if (es_metal_transicion(ZB)) *LB = vB;
    else {
        int cap = obtener_capacidad_capa(eB);
        int h = cap - vB;
        *LB = (h < vB) ? h : vB;
    }
    int k = (*LA < *LB) ? *LA : *LB;
    if (k < 1 && (*LA >= 1 && *LB >= 1)) k = 1;
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