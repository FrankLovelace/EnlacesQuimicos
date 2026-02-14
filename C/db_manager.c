#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "structs.h"
#include "cJSON.h" 

#define RUTA_JSON "/data/base_datos_quimica_final.json"
#define RUTA_PAULING "/data/paulling.json"
#define RUTA_CSV "/data/elementos.csv"

void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
}

AtomoData* cargar_elemento_json(const char* simbolo_buscado) {
     
    FILE *fp = fopen(RUTA_JSON, "r");
    if (!fp) {
        printf("[C ERROR] No se pudo abrir %s\n", RUTA_JSON);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    if (!json) {
        printf("[C ERROR] Fallo al parsear el JSON global.\n");
        free(buffer);
        return NULL;
    }

    cJSON *elem_json = cJSON_GetObjectItemCaseSensitive(json, simbolo_buscado);
    if (!elem_json) {
        printf("[C DEBUG] El elemento '%s' no esta en el JSON.\n", simbolo_buscado);
        free(buffer);
        cJSON_Delete(json);
        return NULL;
    }

    AtomoData *res = calloc(1, sizeof(AtomoData));
    
    cJSON *z_item = cJSON_GetObjectItemCaseSensitive(elem_json, "Z");
    if (z_item) res->Z = z_item->valueint;
    
    strcpy(res->simbolo, simbolo_buscado);
    
    cJSON *estados = cJSON_GetObjectItemCaseSensitive(elem_json, "estados");
    if (estados) {
        cJSON *estado = NULL;
        int contador = 0;
        
        cJSON_ArrayForEach(estado, estados) {
            int c = atoi(estado->string); 
            
            if (c >= 0 && c < 120) {
                res->estados[c].carga = c;
                res->estados[c].mulliken_ev = cJSON_GetObjectItemCaseSensitive(estado, "mulliken_ev")->valuedouble;
                
                res->tiene_datos_carga[c] = 1;
                
                if (c == 0) {
                    res->afinidad_neutra = cJSON_GetObjectItemCaseSensitive(estado, "ea_ev")->valuedouble;
                }
                contador++;
            }
        }
        printf("[C DEBUG] %s -> Se cargaron %d estados ionicos.\n", simbolo_buscado, contador);
    }

    free(buffer);
    cJSON_Delete(json);

    FILE *fp_p = fopen(RUTA_PAULING, "r");
    if (fp_p) {
        fseek(fp_p, 0, SEEK_END);
        long size_p = ftell(fp_p);
        fseek(fp_p, 0, SEEK_SET);
        char *buffer_p = malloc(size_p + 1);
        fread(buffer_p, 1, size_p, fp_p);
        fclose(fp_p);
        buffer_p[size_p] = '\0';

        cJSON *json_p = cJSON_Parse(buffer_p);
        cJSON *val_p = cJSON_GetObjectItemCaseSensitive(json_p, simbolo_buscado);
        
        if (val_p) res->pauling_referencia = val_p->valuedouble;
        else res->pauling_referencia = 0.0;
        
        free(buffer_p);
        cJSON_Delete(json_p);
    }

    return res;
}
void obtener_nombre_completo(const char *simbolo, char *nombre_dest) {
    FILE *fp = fopen(RUTA_CSV, "r");
    if (!fp) {
        printf("[C ERROR] CSV no encontrado: %s\n", RUTA_CSV);
        strcpy(nombre_dest, simbolo);
        return;
    }

    char linea[256];
    int encontrado = 0;
    

    while (fgets(linea, sizeof(linea), fp)) {
        char linea_copia[256];
        strcpy(linea_copia, linea);

        trim(linea_copia);
        if(strlen(linea_copia) == 0) continue; 

        char *token_simbolo = strtok(linea_copia, ",");
        char *token_nombre = strtok(NULL, ",");

        if (token_simbolo && token_nombre) {
            trim(token_simbolo);
            trim(token_nombre);

            if (strcmp(token_simbolo, simbolo) == 0) {
                strcpy(nombre_dest, token_nombre);
                encontrado = 1;
                break;
            }
        }
    }
    fclose(fp);

    if (!encontrado) {
        printf("[C WARNING] No se encontró nombre para '%s' en el CSV.\n", simbolo);
        strcpy(nombre_dest, "Desconocido");
    }
}