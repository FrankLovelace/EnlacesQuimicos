#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

AtomoData* cargar_elemento_json(const char* simbolo);

int main() {
    char simb[5];
    int electrones;

    printf("--- Simulador de Enlace (Datos NIST) ---\n");
    printf("Introduce el simbolo del elemento: ");
    scanf("%s", simb);

    AtomoData *atomo = cargar_elemento_json(simb);

    if (atomo) {
        int max_electrones = obtener_limite_capa(atomo->Z);

        printf("Elemento cargado: %s (Z=%d)\n", atomo->simbolo, atomo->Z);
        
        printf("Introduce el numero de electrones (0-%d): ", max_electrones);
        scanf("%d", &electrones);

        if (electrones < 0 || electrones > max_electrones) {
            printf("Error: Cantidad de electrones fuera de rango permitido (0-%d).\n", max_electrones);
        } else {
            double x = obtener_mulliken_config(atomo, electrones);
            if (x >= 0) {
                printf("\n--- RESULTADO ---\n");
                printf("Configuracion: %s con %d electrones\n", atomo->simbolo, electrones);
                printf("Carga final: %d\n", (atomo->Z - electrones));
                printf("Mulliken: %.4f eV\n", x);
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