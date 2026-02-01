#include <stdio.h>
#include <string.h>
#include "db_types.h"

int buscar_elemento(const char *simbolo_input, const char *ruta_db, ElementoInfo *resultado);

int main() {
    char input_usuario[MAX_SIMBOLO];
    ElementoInfo elemento_encontrado;
    const char *ARCHIVO_DB = "elementos.csv";

    printf("--- Sistema de Busqueda de Elementos ---\n");
    printf("Introduce el simbolo : ");
    
    if (scanf("%s", input_usuario) == 1) {
        
        if (buscar_elemento(input_usuario, ARCHIVO_DB, &elemento_encontrado)) {
            printf("\n--- Resultado ---\n");
            printf("Simbolo       : %s\n", elemento_encontrado.simbolo);
            printf("Nombre        : %s\n", elemento_encontrado.nombre);
            printf("Numero Atomico: %d\n", elemento_encontrado.numero_atomico);
        } else {
            printf("\nEl elemento '%s' no existe en %s.\n", input_usuario, ARCHIVO_DB);
        }
        
    } else {
        printf("Error al leer la entrada.\n");
    }

    // Pausa
    while (getchar() != '\n');
    printf("\nPresiona ENTER para cerrar...");
    getchar(); 

    return 0;
}