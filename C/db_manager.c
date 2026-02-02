#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "cJSON.h" 

#define RUTA_JSON "data/base_datos_quimica_final.json"

void obtener_nombre_completo(const char *simbolo, char *nombre_dest) {
    FILE *fp = fopen("data/elementos.csv", "r");
    if (!fp) {
        strcpy(nombre_dest, "Desconocido");
        return;
    }

    char linea[100];
    while (fgets(linea, sizeof(linea), fp)) {
        char *s = strtok(linea, ",");
        char *n = strtok(NULL, ",");
        if (s && n && strcmp(s, simbolo) == 0) {
            strcpy(nombre_dest, n);
            fclose(fp);
            return;
        }
    }
    strcpy(nombre_dest, "Desconocido");
    fclose(fp);
}

AtomoData* cargar_elemento_json(const char* simbolo_buscado) {
    
    FILE *fp = fopen(RUTA_JSON, "r");
    if (!fp) {
        printf("Error: No se encontro el JSON en %s\n", RUTA_JSON);
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
    cJSON *elem_json = cJSON_GetObjectItemCaseSensitive(json, simbolo_buscado);

    if (!elem_json) {
        free(buffer);
        cJSON_Delete(json);
        return NULL;
    }

    AtomoData *res = calloc(1, sizeof(AtomoData));
    res->Z = cJSON_GetObjectItemCaseSensitive(elem_json, "Z")->valueint;
    strcpy(res->simbolo, simbolo_buscado);
    
    cJSON *estados = cJSON_GetObjectItemCaseSensitive(elem_json, "estados");
    cJSON *estado = NULL;
    
cJSON_ArrayForEach(estado, estados) {
    int c = atoi(estado->string);
    if (c >= 0 && c < 120) {
        res->estados[c].carga = c;
        res->estados[c].ie_ev = cJSON_GetObjectItemCaseSensitive(estado, "ie_ev")->valuedouble;
        res->estados[c].ea_ev = cJSON_GetObjectItemCaseSensitive(estado, "ea_ev")->valuedouble;
        res->estados[c].mulliken_ev = cJSON_GetObjectItemCaseSensitive(estado, "mulliken_ev")->valuedouble;
        res->tiene_datos_carga[c] = 1;
        
        if (c == 0) res->afinidad_neutra = res->estados[c].ea_ev;
    }
}

     FILE *fp_p = fopen("data/paulling.json", "r");
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
        
        if (val_p) {
            res->pauling_referencia = val_p->valuedouble;
        } else {
            res->pauling_referencia = 0.0;
        }
        
        free(buffer_p);
        cJSON_Delete(json_p);
    }

    return res;
}