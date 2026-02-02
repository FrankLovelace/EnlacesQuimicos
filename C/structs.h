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

#endif