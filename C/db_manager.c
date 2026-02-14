#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "structs.h"
#include "cJSON.h" 

#define RUTA_JSON "/data/base_datos_quimica_final.json"

typedef struct {
    const char* s; 
    const char* n; 
    double p;      
} ElemData;

const ElemData TABLA_MAESTRA[] = {
    {"H", "Hidrógeno", 2.20}, {"He", "Helio", 0.00}, {"Li", "Litio", 0.98}, {"Be", "Berilio", 1.57}, {"B", "Boro", 2.04}, {"C", "Carbono", 2.55}, {"N", "Nitrógeno", 3.04}, {"O", "Oxígeno", 3.44}, {"F", "Flúor", 3.98}, {"Ne", "Neón", 0.00},
    {"Na", "Sodio", 0.93}, {"Mg", "Magnesio", 1.31}, {"Al", "Aluminio", 1.61}, {"Si", "Silicio", 1.90}, {"P", "Fósforo", 2.19}, {"S", "Azufre", 2.58}, {"Cl", "Cloro", 3.16}, {"Ar", "Argón", 0.00}, {"K", "Potasio", 0.82}, {"Ca", "Calcio", 1.00},
    {"Sc", "Escandio", 1.36}, {"Ti", "Titanio", 1.54}, {"V", "Vanadio", 1.63}, {"Cr", "Cromo", 1.66}, {"Mn", "Manganeso", 1.55}, {"Fe", "Hierro", 1.83}, {"Co", "Cobalto", 1.88}, {"Ni", "Níquel", 1.91}, {"Cu", "Cobre", 1.90}, {"Zn", "Zinc", 1.65},
    {"Ga", "Galio", 1.81}, {"Ge", "Germanio", 2.01}, {"As", "Arsénico", 2.18}, {"Se", "Selenio", 2.55}, {"Br", "Bromo", 2.96}, {"Kr", "Kriptón", 3.00}, {"Rb", "Rubidio", 0.82}, {"Sr", "Estroncio", 0.95}, {"Y", "Itrio", 1.22}, {"Zr", "Circonio", 1.33},
    {"Nb", "Niobio", 1.60}, {"Mo", "Molibdeno", 2.16}, {"Tc", "Tecnecio", 1.90}, {"Ru", "Rutenio", 2.20}, {"Rh", "Rodio", 2.28}, {"Pd", "Paladio", 2.20}, {"Ag", "Plata", 1.93}, {"Cd", "Cadmio", 1.69}, {"In", "Indio", 1.78}, {"Sn", "Estaño", 1.96},
    {"Sb", "Antimonio", 2.05}, {"Te", "Telurio", 2.10}, {"I", "Yodo", 2.66}, {"Xe", "Xenón", 2.60}, {"Cs", "Cesio", 0.79}, {"Ba", "Bario", 0.89}, {"La", "Lantano", 1.10}, {"Ce", "Cerio", 1.12}, {"Pr", "Praseodimio", 1.13}, {"Nd", "Neodimio", 1.14},
    {"Pm", "Prometio", 1.13}, {"Sm", "Samario", 1.17}, {"Eu", "Europio", 1.20}, {"Gd", "Gadolinio", 1.20}, {"Tb", "Terbio", 1.10}, {"Dy", "Disprosio", 1.22}, {"Ho", "Holmio", 1.23}, {"Er", "Erbio", 1.24}, {"Tm", "Tulio", 1.25}, {"Yb", "Iterbio", 1.10},
    {"Lu", "Lutecio", 1.27}, {"Hf", "Hafnio", 1.30}, {"Ta", "Tántalo", 1.50}, {"W", "Wolframio", 2.36}, {"Re", "Renio", 1.90}, {"Os", "Osmio", 2.20}, {"Ir", "Iridio", 2.20}, {"Pt", "Platino", 2.28}, {"Au", "Oro", 2.54}, {"Hg", "Mercurio", 2.00},
    {"Tl", "Talio", 1.62}, {"Pb", "Plomo", 2.33}, {"Bi", "Bismuto", 2.02}, {"Po", "Polonio", 2.00}, {"At", "Astato", 2.20}, {"Rn", "Radón", 2.20}, {"Fr", "Francio", 0.70}, {"Ra", "Radio", 0.90}, {"Ac", "Actinio", 1.10}, {"Th", "Torio", 1.30},
    {"Pa", "Protactinio", 1.50}, {"U", "Uranio", 1.38}, {"Np", "Neptunio", 1.36}, {"Pu", "Plutonio", 1.28}, {"Am", "Americio", 1.13}, {"Cm", "Curio", 1.28}, {"Bk", "Berkelio", 1.30}, {"Cf", "Californio", 1.30}, {"Es", "Einstenio", 1.30}, {"Fm", "Fermio", 1.30},
    {"Md", "Mendelevio", 1.30}, {"No", "Nobelio", 1.30}, {"Lr", "Laurencio", 1.30}, {"Rf", "Rutherfordio", 1.30}, {"Db", "Dubnio", 1.30}, {"Sg", "Seaborgio", 1.30}, {"Bh", "Bohrio", 1.30}, {"Hs", "Hassio", 1.30}, {"Mt", "Meitnerio", 1.30}, {"Ds", "Darmstatio", 1.30},
    {"Rg", "Roentgenio", 1.30}, {"Cn", "Copernicio", 1.30}, {"Nh", "Nihonio", 1.30}, {"Fl", "Flerovio", 1.30}, {"Mc", "Moscovio", 1.30}, {"Lv", "Livermorio", 1.30}, {"Ts", "Teneso", 1.30}, {"Og", "Oganesón", 1.30},
    {NULL, NULL, 0.0}
};


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
        printf("[C ERROR] Fallo al parsear JSON Mulliken.\n");
        free(buffer);
        return NULL;
    }

    cJSON *elem_json = cJSON_GetObjectItemCaseSensitive(json, simbolo_buscado);
    if (!elem_json) {
        free(buffer); cJSON_Delete(json);
        return NULL;
    }

    AtomoData *res = calloc(1, sizeof(AtomoData));
    res->Z = cJSON_GetObjectItemCaseSensitive(elem_json, "Z")->valueint;
    strcpy(res->simbolo, simbolo_buscado);
    
    cJSON *estados = cJSON_GetObjectItemCaseSensitive(elem_json, "estados");
    if (estados) {
        cJSON *estado = NULL;
        cJSON_ArrayForEach(estado, estados) {
            int c = atoi(estado->string);
            if (c >= 0 && c < 120) {
                res->estados[c].carga = c;
                res->estados[c].mulliken_ev = cJSON_GetObjectItemCaseSensitive(estado, "mulliken_ev")->valuedouble;
                res->tiene_datos_carga[c] = 1;
                if (c == 0) res->afinidad_neutra = cJSON_GetObjectItemCaseSensitive(estado, "ea_ev")->valuedouble;
            }
        }
    }

    res->pauling_referencia = 0.0;
    for (int i = 0; TABLA_MAESTRA[i].s != NULL; i++) {
        if (strcmp(TABLA_MAESTRA[i].s, simbolo_buscado) == 0) {
            res->pauling_referencia = TABLA_MAESTRA[i].p;
            break;
        }
    }

    free(buffer);
    cJSON_Delete(json);
    return res;
}

void obtener_nombre_completo(const char *simbolo, char *nombre_dest) {
    for (int i = 0; TABLA_MAESTRA[i].s != NULL; i++) {
        if (strcmp(TABLA_MAESTRA[i].s, simbolo) == 0) {
            strcpy(nombre_dest, TABLA_MAESTRA[i].n);
            return;
        }
    }
    strcpy(nombre_dest, simbolo);
}