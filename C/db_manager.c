#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "structs.h"
#include "cJSON.h" 

#define RUTA_JSON "/data/base_datos_quimica_final.json"
#define RUTA_PAULING "/data/paulling.json"

typedef struct {
    const char* simbolo;
    const char* nombre;
} ElementoNombre;

const ElementoNombre TABLA_NOMBRES[] = {
    {"H", "Hidrógeno"}, {"He", "Helio"}, {"Li", "Litio"}, {"Be", "Berilio"}, {"B", "Boro"}, {"C", "Carbono"}, {"N", "Nitrógeno"}, {"O", "Oxígeno"}, {"F", "Flúor"}, {"Ne", "Neón"},
    {"Na", "Sodio"}, {"Mg", "Magnesio"}, {"Al", "Aluminio"}, {"Si", "Silicio"}, {"P", "Fósforo"}, {"S", "Azufre"}, {"Cl", "Cloro"}, {"Ar", "Argón"}, {"K", "Potasio"}, {"Ca", "Calcio"},
    {"Sc", "Escandio"}, {"Ti", "Titanio"}, {"V", "Vanadio"}, {"Cr", "Cromo"}, {"Mn", "Manganeso"}, {"Fe", "Hierro"}, {"Co", "Cobalto"}, {"Ni", "Níquel"}, {"Cu", "Cobre"}, {"Zn", "Zinc"},
    {"Ga", "Galio"}, {"Ge", "Germanio"}, {"As", "Arsénico"}, {"Se", "Selenio"}, {"Br", "Bromo"}, {"Kr", "Kriptón"}, {"Rb", "Rubidio"}, {"Sr", "Estroncio"}, {"Y", "Itrio"}, {"Zr", "Circonio"},
    {"Nb", "Niobio"}, {"Mo", "Molibdeno"}, {"Tc", "Tecnecio"}, {"Ru", "Rutenio"}, {"Rh", "Rodio"}, {"Pd", "Paladio"}, {"Ag", "Plata"}, {"Cd", "Cadmio"}, {"In", "Indio"}, {"Sn", "Estaño"},
    {"Sb", "Antimonio"}, {"Te", "Telurio"}, {"I", "Yodo"}, {"Xe", "Xenón"}, {"Cs", "Cesio"}, {"Ba", "Bario"}, {"La", "Lantano"}, {"Ce", "Cerio"}, {"Pr", "Praseodimio"}, {"Nd", "Neodimio"},
    {"Pm", "Prometio"}, {"Sm", "Samario"}, {"Eu", "Europio"}, {"Gd", "Gadolinio"}, {"Tb", "Terbio"}, {"Dy", "Disprosio"}, {"Ho", "Holmio"}, {"Er", "Erbio"}, {"Tm", "Tulio"}, {"Yb", "Iterbio"},
    {"Lu", "Lutecio"}, {"Hf", "Hafnio"}, {"Ta", "Tántalo"}, {"W", "Wolframio"}, {"Re", "Renio"}, {"Os", "Osmio"}, {"Ir", "Iridio"}, {"Pt", "Platino"}, {"Au", "Oro"}, {"Hg", "Mercurio"},
    {"Tl", "Talio"}, {"Pb", "Plomo"}, {"Bi", "Bismuto"}, {"Po", "Polonio"}, {"At", "Astato"}, {"Rn", "Radón"}, {"Fr", "Francio"}, {"Ra", "Radio"}, {"Ac", "Actinio"}, {"Th", "Torio"},
    {"Pa", "Protactinio"}, {"U", "Uranio"}, {"Np", "Neptunio"}, {"Pu", "Plutonio"}, {"Am", "Americio"}, {"Cm", "Curio"}, {"Bk", "Berkelio"}, {"Cf", "Californio"}, {"Es", "Einstenio"}, {"Fm", "Fermio"},
    {"Md", "Mendelevio"}, {"No", "Nobelio"}, {"Lr", "Laurencio"}, {"Rf", "Rutherfordio"}, {"Db", "Dubnio"}, {"Sg", "Seaborgio"}, {"Bh", "Bohrio"}, {"Hs", "Hassio"}, {"Mt", "Meitnerio"}, {"Ds", "Darmstatio"},
    {"Rg", "Roentgenio"}, {"Cn", "Copernicio"}, {"Nh", "Nihonio"}, {"Fl", "Flerovio"}, {"Mc", "Moscovio"}, {"Lv", "Livermorio"}, {"Ts", "Teneso"}, {"Og", "Oganesón"},
    {NULL, NULL}
};


void trim(char * s) {
    if (!s) return;
    char * p = s;
    int l = strlen(p);
    while(l > 0 && isspace(p[l - 1])) p[--l] = 0;
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
                cJSON *mulliken = cJSON_GetObjectItemCaseSensitive(estado, "mulliken_ev");
                if (mulliken) {
                    res->estados[c].mulliken_ev = mulliken->valuedouble;
                } else {
                    res->estados[c].mulliken_ev = 0.0;
                }
                res->tiene_datos_carga[c] = 1;
                if (c == 0) {
                    cJSON *ea = cJSON_GetObjectItemCaseSensitive(estado, "ea_ev");
                    if (ea) {
                        res->afinidad_neutra = ea->valuedouble;
                    } else {
                        res->afinidad_neutra = 0.0;
                    }
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
        if (json_p) {
            cJSON *val_p = cJSON_GetObjectItemCaseSensitive(json_p, simbolo_buscado);
            if (val_p) res->pauling_referencia = val_p->valuedouble;
            else res->pauling_referencia = 0.0;
            cJSON_Delete(json_p);
        }
        free(buffer_p);
    }

    return res;
}

void obtener_nombre_completo(const char *simbolo, char *nombre_dest) {
    for (int i = 0; TABLA_NOMBRES[i].simbolo != NULL; i++) {
        if (strcmp(TABLA_NOMBRES[i].simbolo, simbolo) == 0) {
            strcpy(nombre_dest, TABLA_NOMBRES[i].nombre);
            return;
        }
    }
    strcpy(nombre_dest, simbolo);
}