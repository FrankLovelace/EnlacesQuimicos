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

int obtener_capacidad_capa(int num_electrones) {
    if (num_electrones <= 2) return 2;
    return 8;
}

int calcular_valencia_v(int num_electrones) {
    int cores[] = {86, 54, 36, 18, 10, 2, 0};
    for (int i = 0; i < 7; i++) {
        if (num_electrones > cores[i]) {
            return num_electrones - cores[i];
        }
    }
    return num_electrones;
}


double obtener_mulliken_config(AtomoData *atomo, int num_electrones) {
    int carga = atomo->Z - num_electrones;

    if (carga >= 0) {
        if (carga < 120 && atomo->tiene_datos_carga[carga]) {
            return atomo->estados[carga].mulliken_ev;
        }
        return -1.0; 
    }
    else {
        int max_e = obtener_limite_capa(atomo->Z);
        int limite_negativo = atomo->Z - max_e;

        if (carga < limite_negativo) return -1.0;
        if (carga == limite_negativo) return 0.0; 

        return atomo->afinidad_neutra / 2.0;
    }
}



double derivada_edo(double x, double y) {
    double numerador = 0.4 * y;
    double denominador = 1.0 + (0.1 * x);
    double saturacion = 1.0 - (y / 5.0); 
    
    return (numerador / denominador) * saturacion;
}

double resolver_pauling_rk4(double x_target) {
    double x = 2.42; 
    double y = 0.82; 
    
    if (x_target < x) return 0.7; 

    double h = 0.05; 
    
    while (x < x_target) {
        if (x + h > x_target) {
            h = x_target - x;
        }

        double k1 = h * derivada_edo(x, y);
        double k2 = h * derivada_edo(x + 0.5*h, y + 0.5*k1);
        double k3 = h * derivada_edo(x + 0.5*h, y + 0.5*k2);
        double k4 = h * derivada_edo(x + h, y + k3);

        y = y + (k1 + 2*k2 + 2*k3 + k4) / 6.0;
        x = x + h;
    }

    if (y > 4.0) return 4.0; 
    return y;
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

const char* determinar_tipo_enlace_mulliken(double delta_mulliken, double promedio_mulliken) {
    if (promedio_mulliken < 4.89) {
        return "Metálico";
    } 
    if (delta_mulliken > 4.10) {
        return "Iónico";
    } 
    if (delta_mulliken > 0.61) {
        return "Covalente Polar";
    } 
    return "Covalente No Polar";
}

double calcular_referencia_pauling(double x) {
    if (x < 0) return 0.0;
    return (0.374 * x) + 0.17; 
}


int calcular_multiplicidad(int eA, int eB, int *LA, int *LB) {
    int VA = calcular_valencia_v(eA);
    int CapA = obtener_capacidad_capa(eA);
    int HA = CapA - VA;
    if (HA < 0) HA = 0;
    *LA = (VA < HA) ? VA : HA;

    int VB = calcular_valencia_v(eB);
    int CapB = obtener_capacidad_capa(eB);
    int HB = CapB - VB;
    if (HB < 0) HB = 0;
    *LB = (VB < HB) ? VB : HB;

    int k = (*LA < *LB) ? *LA : *LB;
    if (k > 3) k = 3; 
    return k;
}

const char* nombre_multiplicidad(int k) {
    if (k == 1) return "Simple (sigma)";
    if (k == 2) return "Doble (sigma + pi)";
    if (k >= 3) return "Triple (sigma + 2pi)";
    return "No definido / Coordinado";
}