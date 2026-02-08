#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"

void procesar_referencia_pauling(AtomoData *atomo, int electrones, double m_actual) {
    int carga = atomo->Z - electrones;
    double p_ref = atomo->pauling_referencia;

    printf("  -> Mulliken Actual: %.4f eV\n", m_actual);
    if (carga == 0) printf("  -> Escala Pauling: = %.2f\n", p_ref);
    else if (carga > 0) printf("  -> Escala Pauling: > %.2f\n", p_ref);
    else printf("  -> Escala Pauling: < %.2f\n", p_ref);
}

void flujo_entrada_electrones(AtomoData *atomo, int *electrones, double *mulliken) {
    while (1) {
        printf(" Introduce electrones para %s (0-%d): ", atomo->simbolo, atomo->Z);
        if (scanf("%d", electrones) != 1) {
            while (getchar() != '\n');
            continue;
        }

        *mulliken = obtener_mulliken_config(atomo, *electrones);

        if (*mulliken == -2.0) printf(" [!] Error: No se permiten aniones (electrones > Z).\n");
        else if (*mulliken == -1.0) printf(" [!] Sin informacion: NIST no tiene datos para esa carga.\n");
        else {
            procesar_referencia_pauling(atomo, *electrones, *mulliken);
            break; 
        }
    }
}

int main() {
    char simbA[5], simbB[5], nomA[50], nomB[50];
    int eA, eB, LA, LB;
    double mA, mB;

    printf("--- Simulador de Enlaces VAK (Van Arkel-Ketelaar) ---\n\n");

    printf("Simbolo A: "); scanf("%s", simbA);
    AtomoData *atA = cargar_elemento_json(simbA);
    if (!atA) { printf("Error: %s no encontrado.\n", simbA); return 1; }
    obtener_nombre_completo(atA->simbolo, nomA);
    printf(" >>> %s (%s), Z=%d\n", nomA, atA->simbolo, atA->Z);
    flujo_entrada_electrones(atA, &eA, &mA);

    printf("\nSimbolo B: "); scanf("%s", simbB);
    AtomoData *atB = cargar_elemento_json(simbB);
    if (!atB) { free(atA); printf("Error: %s no encontrado.\n", simbB); return 1; }
    obtener_nombre_completo(atB->simbolo, nomB);
    printf(" >>> %s (%s), Z=%d\n", nomB, atB->simbolo, atB->Z);
    flujo_entrada_electrones(atB, &eB, &mB);

    double vak_y = fabs(mA - mB);       
    double vak_x = (mA + mB) / 2.0;     
    
    double ic = calcular_porcentaje_ic(vak_y);
    int k = calcular_multiplicidad(eA, eB, atA->Z, atB->Z, &LA, &LB);

    const char* tipo_enlace = determinar_tipo_enlace_vak(vak_x, vak_y, mA, mB, atA->Z, atB->Z, k);

    printf("\n========================================\n");
    printf("      ANALISIS VAK (Mulliken eV)\n");
    printf("========================================\n");
    printf(" Componentes: %s + %s\n", nomA, nomB);
    printf(" Coordenada X (Promedio) : %.4f eV\n", vak_x);
    printf(" Coordenada Y (Delta)    : %.4f eV\n", vak_y);
    printf(" Caracter Ionico         : %.2f %%\n", ic);
    printf("----------------------------------------\n");
    printf(" Brazos Libres (L)       : %s=%d, %s=%d\n", atA->simbolo, LA, atB->simbolo, LB);
    printf(" Multiplicidad (k)       : %d -> %s\n", k, nombre_multiplicidad(k));
    printf(" CLASIFICACION VAK       : %s\n", tipo_enlace);
    printf("========================================\n");

    free(atA); free(atB);
    printf("\nPresiona Enter para salir...");
    fflush(stdin); getchar(); getchar();
    return 0;
}