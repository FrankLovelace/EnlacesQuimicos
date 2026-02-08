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
    double pauling_referencia; 
    EstadoIonico estados[120]; 
    int tiene_datos_carga[120]; 
} AtomoData;

AtomoData* cargar_elemento_json(const char* simbolo);
void obtener_nombre_completo(const char *simbolo, char *nombre_dest);

int es_metal_transicion(int Z);
int es_lantanido_actinido(int Z);
int es_gas_noble_z(int Z);
int obtener_capacidad_capa(int num_electrones);
int calcular_valencia_v(int num_electrones, int Z);

double obtener_mulliken_config(AtomoData *atomo, int num_electrones);
double calcular_delta_chi(double chiA, double chiB);
double calcular_promedio_chi(double chiA, double chiB);
double calcular_porcentaje_ic(double delta_mulliken);

const char* determinar_tipo_enlace_mulliken(double delta_m, double prom_m, double mA, double mB, int ZA, int ZB, int k);
const char* determinar_tipo_enlace_vak(double x_promedio, double y_delta, double mA, double mB, int ZA, int ZB, int k);
int calcular_multiplicidad(int eA, int eB, int ZA, int ZB, int *LA, int *LB);
const char* nombre_multiplicidad(int k);
const char* clasificar_ion(int carga);



#endif