#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

int main() {
    char simbA[5], simbB[5], nomA[50], nomB[50];
    int eA, eB, LA, LB;
    double chiA, chiB;

    printf("--- Simulador de Enlaces NIST ---\n\n");

    printf("Simbolo Elemento A: "); scanf("%s", simbA);
    AtomoData *atA = cargar_elemento_json(simbA);
    if (!atA) return 1;
    obtener_nombre_completo(atA->simbolo, nomA);
    printf(" > %s cargado (Z=%d)\n", nomA, atA->Z);
    
    printf(" Introduce electrones (0-%d): ", obtener_limite_capa(atA->Z));
    scanf("%d", &eA);
    double m_evA = obtener_mulliken_config(atA, eA);
chiA = convertir_a_pauling(m_evA, atA->Z, eA); 
printf("  -> Electronegatividad Pauling: %.4f\n\n", chiA);

    printf("Simbolo Elemento B: "); scanf("%s", simbB);
    AtomoData *atB = cargar_elemento_json(simbB);
    if (!atB) { free(atA); return 1; }
    obtener_nombre_completo(atB->simbolo, nomB);
    printf(" > %s cargado (Z=%d)\n", nomB, atB->Z);

    printf(" Introduce electrones (0-%d): ", obtener_limite_capa(atB->Z));
    scanf("%d", &eB);
    double m_evB = obtener_mulliken_config(atB, eB);
chiB = convertir_a_pauling(m_evB, atB->Z, eB); 
printf("  -> Electronegatividad Pauling: %.4f\n", chiB);

    double delta = calcular_delta_chi(chiA, chiB);
    double prom = calcular_promedio_chi(chiA, chiB);
    double ic = calcular_porcentaje_ic(delta);
    int k = calcular_multiplicidad(eA, eB, &LA, &LB);

    printf("\n========================================\n");
    printf("      RESULTADOS DEL ENLACE\n");
    printf("========================================\n");
    printf(" A. Diferencia (Delta Chi) : %.4f\n", delta);
    printf(" B. Promedio (Chi barra)   : %.4f\n", prom);
    printf(" C. Caracter Ionico (%%IC)  : %.2f %%\n", ic);
    printf("----------------------------------------\n");
    printf(" Brazos Libres (L): %s=%d, %s=%d\n", simbA, LA, simbB, LB);
    printf(" Multiplicidad (k): %d -> %s\n", k, nombre_multiplicidad(k));
    printf(" Tipo de Enlace   : %s\n", determinar_tipo_enlace(delta, prom));
    printf("========================================\n");

    free(atA); free(atB);
    printf("\nPresiona Enter para cerrar...");
    fflush(stdin); getchar(); getchar();
    return 0;
}