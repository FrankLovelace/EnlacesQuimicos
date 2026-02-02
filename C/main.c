#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

void imprimir_atomo_stats(AtomoData *atomo, int electrones, double *chi_pauling) {
    char nombre[50];
    obtener_nombre_completo(atomo->simbolo, nombre);
    double m_ev = obtener_mulliken_config(atomo, electrones);
    *chi_pauling = convertir_a_pauling(m_ev);
    int carga = atomo->Z - electrones;

    printf("\n--- Datos de %s (%s) ---\n", nombre, atomo->simbolo);
    printf(" Carga: %d (%s) | Pauling: %.4f\n", carga, clasificar_ion(carga), *chi_pauling);
}

int main() {
    char simbA[5], simbB[5];
    int eA, eB;
    double chiA, chiB;

    printf("--- Simulador de Enlaces de 2 Elementos ---\n");

    printf("Simbolo Elemento A: "); scanf("%s", simbA);
    AtomoData *atA = cargar_elemento_json(simbA);
    if (!atA) return 1;
    printf("Electrones para %s (0-%d): ", simbA, obtener_limite_capa(atA->Z));
    scanf("%d", &eA);

    printf("Simbolo Elemento B: "); scanf("%s", simbB);
    AtomoData *atB = cargar_elemento_json(simbB);
    if (!atB) { free(atA); return 1; }
    printf("Electrones para %s (0-%d): ", simbB, obtener_limite_capa(atB->Z));
    scanf("%d", &eB);

    imprimir_atomo_stats(atA, eA, &chiA);
    imprimir_atomo_stats(atB, eB, &chiB);

    double delta_chi = calcular_delta_chi(chiA, chiB);
    double promedio_chi = calcular_promedio_chi(chiA, chiB);
    double porcentaje_ic = calcular_porcentaje_ic(delta_chi);
    const char* tipo = determinar_tipo_enlace(delta_chi, promedio_chi);

    printf("\n========================================\n");
    printf("  ANALISIS DEL ENLACE\n");
    printf("========================================\n");
    printf(" A. Diferencia (Delta Chi) : %.4f\n", delta_chi);
    printf(" B. Promedio (Chi barra)   : %.4f\n", promedio_chi);
    printf(" C. Caracter Ionico (%%IC)  : %.2f %%\n", porcentaje_ic);
    printf("----------------------------------------\n");
    printf(" TIPO DE ENLACE ESTIMADO: %s\n", tipo);
    printf("========================================\n");

    free(atA);
    free(atB);
    printf("\nPresiona Enter para cerrar...");
    fflush(stdin); getchar(); getchar();

    return 0;
}