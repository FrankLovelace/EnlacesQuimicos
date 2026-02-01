#ifndef DB_TYPES_H
#define DB_TYPES_H

#define MAX_SIMBOLO 5
#define MAX_NOMBRE 50

typedef struct {
    char simbolo[MAX_SIMBOLO];
    char nombre[MAX_NOMBRE];
    int numero_atomico;
} ElementoInfo;

#endif