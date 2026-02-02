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

int obtener_limite_capa(int Z);
double obtener_mulliken_config(AtomoData *atomo, int num_electrones);

#endif