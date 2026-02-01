#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_types.h"

void limpiar_linea(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}

int buscar_elemento(const char *simbolo_input, const char *ruta_db, ElementoInfo *resultado) {
    FILE *archivo = fopen(ruta_db, "r");
    if (!archivo) {
        printf("Error: No se pudo abrir la base de datos %s\n", ruta_db);
        return 0;
    }

    char linea[150]; 
    char *token;

    while (fgets(linea, sizeof(linea), archivo)) {
        limpiar_linea(linea);

        token = strtok(linea, ",");
        
        if (token != NULL) {
            if (strcmp(token, simbolo_input) == 0) {
                strcpy(resultado->simbolo, token);
                
                token = strtok(NULL, ",");
                if (token) strcpy(resultado->nombre, token);
                else strcpy(resultado->nombre, "Desconocido");

                token = strtok(NULL, ",");
                if (token) {
                    resultado->numero_atomico = atoi(token); 
                } else {
                    resultado->numero_atomico = 0;
                }

                fclose(archivo);
                return 1;
            }
        }
    }

    fclose(archivo);
    return 0;
}