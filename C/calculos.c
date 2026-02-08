#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "structs.h"

int es_metal_transicion(int Z) {
    return ((Z >= 21 && Z <= 30) || (Z >= 39 && Z <= 48) || (Z >= 71 && Z <= 80));
}

int es_lantanido_actinido(int Z) {
    return ((Z >= 57 && Z <= 70) || (Z >= 89 && Z <= 102));
}

int es_gas_noble_z(int Z) {
    return (Z == 2 || Z == 10 || Z == 18 || Z == 36 || Z == 54 || Z == 86);
}

int es_inductor_hipervalencia(int Z) {
    return (Z == 8 || Z == 9 || Z == 17); // O, F, Cl
}

int es_noble_pesado(int Z) {
    return (Z == 36 || Z == 54 || Z == 86); // Kr, Xe, Rn
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

    if (es_gas_noble_z(Z) && num_electrones == Z) return 0;

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

    if (carga < 0) return -2.0; 

    if (carga < 120 && atomo->tiene_datos_carga[carga]) {
        return atomo->estados[carga].mulliken_ev;
    }

    return -1.0; 
}


double calcular_delta_chi(double chiA, double chiB) { return fabs(chiA - chiB); }

double calcular_promedio_chi(double chiA, double chiB) { return (chiA + chiB) / 2.0; }

double calcular_porcentaje_ic(double delta_mulliken) {
    double dp = 0.374 * delta_mulliken;
    double ic_pauling = (1.0 - exp(-0.25 * pow(dp, 2))) * 100.0;
    double ic_hannay = (16.0 * dp) + (3.5 * pow(dp, 2));
    
    if (ic_hannay > 100.0) ic_hannay = 100.0;

    double ic_final = 0.0;
    if (dp < 1.6) ic_final = ic_pauling;
    else if (dp > 2.4) ic_final = ic_hannay;
    else {
        double w = (dp - 1.6) / (2.4 - 1.6);
        ic_final = ((1.0 - w) * ic_pauling) + (w * ic_hannay);
    }

    if (ic_final > 100.0) return 100.0;
    if (ic_final < 0.0) return 0.0;
    return ic_final;
}

const char* determinar_tipo_enlace_mulliken(double delta_m, double prom_m, double mA, double mB, int ZA, int ZB, int k) {
    if (k <= 0) {
        if (es_gas_noble_z(ZA) || es_gas_noble_z(ZB)) return "Fuerzas de Van der Waals (Inerte)";
        return "Sin Enlace (Capa Cerrada / Repulsión)";
    }

    if (mA < 1.0 || mB < 1.0) return "Inestable (Repulsión Electrostática)";

    if ((es_noble_pesado(ZA) && es_inductor_hipervalencia(ZB)) || 
        (es_noble_pesado(ZB) && es_inductor_hipervalencia(ZA))) {
        return "Covalente Polar (Hipervalente)";
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

        if (*LA == 0 && es_noble_pesado(ZA) && es_inductor_hipervalencia(ZB)) {
            *LA = 1; 
        }
    }

    if (es_metal_transicion(ZB) || es_lantanido_actinido(ZB)) {
        *LB = vB;
    } else {
        int cap = (eB <= 2) ? 2 : 8;
        int huecos = cap - vB;
        *LB = (huecos < vB) ? huecos : vB;

        if (*LB == 0 && es_noble_pesado(ZB) && es_inductor_hipervalencia(ZA)) {
            *LB = 1;
        }
    }

    if (*LA < 0) *LA = 0;
    if (*LB < 0) *LB = 0;
    
    int k = (*LA < *LB) ? *LA : *LB;

    if (es_metal_transicion(ZA) || es_metal_transicion(ZB)) {
        if (k >= 1) k = 1; 
    }

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
const char* determinar_tipo_enlace_vak(double x, double y, double mA, double mB, int ZA, int ZB, int k) {
    
    if (k <= 0) {
        if (es_gas_noble_z(ZA) || es_gas_noble_z(ZB)) return "Fuerzas de Van der Waals (Inerte)";
        return "Sin Enlace (Repulsión / Capa Cerrada)";
    }

    if (mA < 1.0 || mB < 1.0) return "Inestable (Repulsión Electrostática)";

    if ((es_noble_pesado(ZA) && es_inductor_hipervalencia(ZB)) || 
        (es_noble_pesado(ZB) && es_inductor_hipervalencia(ZA))) {
        return "Covalente Polar (Hipervalente)";
    }


    if (y > 5.0) {
        return "Iónico (Enlace Principalmente Electrostatico)";
    }

    if (y < 1.5) {
        if (x < 6.0) {
            return "Metálico / Aleación"; 
        } else {
            return "Covalente No Polar";  
        }
    }

    
    if (x < 6.0) {
        return "Covalente Polar (Sólido / Red Iónica)"; 
    } else {
        return "Covalente Polar (Molecular)";
    }
}