#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    int carga;
    double ie_ev;
    double ea_ev;
    double mulliken_ev;
} EstadoIonico;

typedef struct {
    int Z;
    char simbolo[5];
    double afinidad_neutra; 
    EstadoIonico estados[120]; 
    int tiene_datos_carga[120]; 
} AtomoData;

AtomoData* cargar_elemento_json(const char* simbolo);
void obtener_nombre_completo(const char *simbolo, char *nombre_dest);
int obtener_limite_capa(int Z);
double obtener_mulliken_config(AtomoData *atomo, int num_electrones);
double convertir_a_pauling(double mulliken_ev);
const char* clasificar_ion(int carga);

double calcular_delta_chi(double chiA, double chiB);
double calcular_promedio_chi(double chiA, double chiB);
double calcular_porcentaje_ic(double delta_chi);
const char* determinar_tipo_enlace(double delta_chi, double promedio_chi);

#endif