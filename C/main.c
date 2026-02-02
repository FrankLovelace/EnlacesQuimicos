#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

int main() {
    char simb[5];
    char nombre_completo[50];
    int electrones;

    printf("--- Simulador de Enlace Quimico ---\n");
    printf("Introduce el simbolo del elemento: ");
    scanf("%s", simb);

    AtomoData *atomo = cargar_elemento_json(simb);

    if (atomo) {
        obtener_nombre_completo(atomo->simbolo, nombre_completo);
        int max_e = obtener_limite_capa(atomo->Z);

        printf("\nElemento: %s (%s)\n", nombre_completo, atomo->simbolo);
        printf("Numero Atomico (Z): %d\n", atomo->Z);
        printf("Introduce el numero de electrones (0-%d): ", max_e);
        scanf("%d", &electrones);

        if (electrones < 0 || electrones > max_e) {
            printf("Error: Cantidad de electrones invalida.\n");
        } else {
            double m_ev = obtener_mulliken_config(atomo, electrones);
            
            if (m_ev >= 0) {
                int carga_final = atomo->Z - electrones;
                double p_scale = convertir_a_pauling(m_ev);
                const char* tipo = clasificar_ion(carga_final);

                printf("\n========================================\n");
                printf("  RESULTADOS DE LA SIMULACION\n");
                printf("========================================\n");
                printf(" Nombre del Elemento : %s\n", nombre_completo);
                printf(" Carga Electrica     : %d (%s)\n", carga_final, tipo);
                printf(" Mulliken (eV)       : %.4f\n", m_ev);
                printf(" Escala de Pauling   : %.4f\n", p_scale);
                printf("========================================\n");
            }
        }
        free(atomo);
    } else {
        printf("Error: Elemento %s no encontrado.\n", simb);
    }

    printf("\nPresiona Enter para cerrar...");
    fflush(stdin);
    getchar(); getchar();

    return 0;
}