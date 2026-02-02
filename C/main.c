#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "structs.h"

void procesar_referencia_pauling(AtomoData *atomo, int electrones, double m_actual) {
    int carga = atomo->Z - electrones;
    double p_ref = atomo->pauling_referencia;

    printf("  -> Mulliken Actual: %.4f eV\n", m_actual);
    
    if (carga == 0) {
        printf("  -> Escala Pauling: = %.2f\n", p_ref);
    } else if (carga > 0) {
        printf("  -> Escala Pauling: > %.2f\n", p_ref);
    } else {
        printf("  -> Escala Pauling: < %.2f\n", p_ref);
    }
}

int main() {
    char simbA[5], simbB[5], nomA[50], nomB[50];
    int eA, eB, LA, LB;
    double mA, mB;

    printf("--- Simulador de Enlaces (Escala Mulliken eV) ---\n\n");

    printf("Simbolo A: "); scanf("%s", simbA);
    AtomoData *atA = cargar_elemento_json(simbA);
    if (!atA) return 1;
    obtener_nombre_completo(atA->simbolo, nomA);
    printf(" > %s (Z=%d)\n", nomA, atA->Z);
    printf(" Electrones (0-%d): ", obtener_limite_capa(atA->Z)); scanf("%d", &eA);
    mA = obtener_mulliken_config(atA, eA);
    procesar_referencia_pauling(atA, eA, mA);

    printf("\nSimbolo B: "); scanf("%s", simbB);
    AtomoData *atB = cargar_elemento_json(simbB);
    if (!atB) { free(atA); return 1; }
    obtener_nombre_completo(atB->simbolo, nomB);
    printf(" > %s (Z=%d)\n", nomB, atB->Z);
    printf(" Electrones (0-%d): ", obtener_limite_capa(atB->Z)); scanf("%d", &eB);
    mB = obtener_mulliken_config(atB, eB);
    procesar_referencia_pauling(atB, eB, mB);

    double delta_m = fabs(mA - mB);
    double prom_m = (mA + mB) / 2.0;
    
    double ic = calcular_porcentaje_ic( (0.374 * delta_m) );
    int k = calcular_multiplicidad(eA, eB, &LA, &LB);

    printf("\n========================================\n");
    printf("      RESULTADOS DEL ENLACE (eV)\n");
    printf("========================================\n");
    printf(" A. Delta Mulliken      : %.4f eV\n", delta_m);
    printf(" B. Promedio Mulliken   : %.4f eV\n", prom_m);
    printf(" C. Caracter Ionico     : %.2f %%\n", ic);
    printf("----------------------------------------\n");
    printf(" Brazos (L): %s=%d, %s=%d | k=%d\n", simbA, LA, simbB, LB, k);
    printf(" Tipo de Enlace: %s\n", determinar_tipo_enlace_mulliken(delta_m, prom_m));
    printf("========================================\n");

    free(atA); free(atB);
    printf("\nPresiona Enter para cerrar...");
    fflush(stdin); getchar(); getchar();
    return 0;
}